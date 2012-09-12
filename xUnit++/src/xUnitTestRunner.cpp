#include "xUnitTestRunner.h"
#include <atomic>
#include <chrono>
#include <future>
#include <limits>
#include <mutex>
#include <random>
#include <stdexcept>
#include <vector>
#include "IOutput.h"
#include "TestCollection.h"
#include "TestDetails.h"
#include "xUnitAssert.h"
#include "xUnitTime.h"

namespace xUnitpp
{

class TestRunner::Impl
{
public:
    Impl(IOutput &testReporter)
        : mTestReporter(testReporter)
    {
    }

    void OnTestStart(const TestDetails &details)
    {
        std::lock_guard<std::mutex> guard(mStartMtx);
        mTestReporter.ReportStart(details);
    }

    void OnTestFailure(const TestDetails &details, const std::string &message, const LineInfo &lineInfo)
    {
        std::lock_guard<std::mutex> guard(mFailureMtx);
        mTestReporter.ReportFailure(details, message, lineInfo);
    }

    void OnTestSkip(const TestDetails &details, const std::string &reason)
    {
        mTestReporter.ReportSkip(details, reason);
    }

    void OnTestFinish(const TestDetails &details, Time::Duration time)
    {
        std::lock_guard<std::mutex> guard(mFinishMtx);
        mTestReporter.ReportFinish(details, time);
    }


    void OnAllTestsComplete(int total, int skipped, int failed, Time::Duration totalTime)
    {
        mTestReporter.ReportAllTestsComplete(total, skipped, failed, totalTime);
    }

private:
    Impl(const Impl &);
    Impl &operator=(Impl);

private:
    IOutput &mTestReporter;

    std::mutex mStartMtx;
    std::mutex mFailureMtx;
    std::mutex mFinishMtx;
};

TestRunner::TestRunner(IOutput &testReporter)
    : mImpl(new Impl(testReporter))
{
}

int TestRunner::RunTests(std::function<bool(const TestDetails &)> filter, const std::vector<xUnitTest> &tests, Time::Duration maxTestRunTime, size_t maxConcurrent)
{
    auto timeStart = std::chrono::system_clock::now();

    if (maxConcurrent == 0)
    {
        maxConcurrent = std::numeric_limits<decltype(maxConcurrent)>::max();
    }

    class ThreadCounter
    {
    public:
        ThreadCounter(size_t maxThreads)
            : maxThreads(maxThreads)
            , activeThreads(0)
        {
        }

        void operator++()
        {
            std::unique_lock<std::mutex> lock(mtx);
            condition.wait(lock, [&]() { return activeThreads < maxThreads; });

            ++activeThreads;
        }

        void operator--()
        {
            --activeThreads;
            condition.notify_one();
        }

    private:
        size_t maxThreads;
        std::atomic<size_t> activeThreads;
        std::mutex mtx;
        std::condition_variable condition;
    } threadCounter(maxConcurrent);

    std::atomic<int> failedTests = 0;
    int skippedTests = 0;

    std::vector<xUnitTest> activeTests;
    std::copy_if(tests.begin(), tests.end(), std::back_inserter(activeTests), [&filter](const xUnitTest &test) { return filter(test.TestDetails()); });

    std::vector<std::future<void>> futures;
    for (auto &test : activeTests)
    {
        {
            auto skip = test.TestDetails().Attributes.find("Skip");
            if (skip != test.TestDetails().Attributes.end())
            {
                skippedTests++;
                mImpl->OnTestSkip(test.TestDetails(), skip->second);
                continue;
            }
        }

        futures.push_back(std::async([&]()
            {
                struct CounterGuard
                {
                    CounterGuard(ThreadCounter &tc)
                        : tc(tc)
                    {
                        ++tc;
                    }

                    ~CounterGuard()
                    {
                        --tc;
                    }

                private:
                    CounterGuard &operator =(const CounterGuard &) { throw std::logic_error("not supported"); }
                    ThreadCounter &tc;
                } counterGuard(threadCounter);

                auto actualTest = [&](bool reportEnd) -> Time::TimeStamp
                    {
                        bool failed = false;
                        Time::TimeStamp testStart;

                        auto CheckNonFatalErrors = [&]()
                        {
                            if (!failed && !test.NonFatalFailures().empty())
                            {
                                failed = true;
                                for (const auto &assert : test.NonFatalFailures())
                                {
                                    mImpl->OnTestFailure(test.TestDetails(), assert.what(), assert.LineInfo());
                                }
                            }
                        };
                        
                        try
                        {
                            mImpl->OnTestStart(test.TestDetails());

                            testStart = Time::Clock::now();
                            test.Run();
                        }
                        catch (const xUnitAssert &e)
                        {
                            CheckNonFatalErrors();
                            mImpl->OnTestFailure(test.TestDetails(), e.what(), e.LineInfo());
                            failed = true;
                        }
                        catch (const std::exception &e)
                        {
                            CheckNonFatalErrors();
                            mImpl->OnTestFailure(test.TestDetails(), e.what(), LineInfo::empty());
                            failed = true;
                        }
                        catch (...)
                        {
                            CheckNonFatalErrors();
                            mImpl->OnTestFailure(test.TestDetails(), "Unknown exception caught: test has crashed", LineInfo::empty());
                            failed = true;
                        }

                        CheckNonFatalErrors();

                        if (failed)
                        {
                            ++failedTests;
                        }

                        if (reportEnd)
                        {
                            mImpl->OnTestFinish(test.TestDetails(), Time::ToDuration(Time::Clock::now() - testStart));
                        }

                        return testStart;
                    };

                auto testTimeLimit = test.TestDetails().TimeLimit;
                if (testTimeLimit < Time::Duration::zero())
                {
                    testTimeLimit = maxTestRunTime;
                }

                if (testTimeLimit > Time::Duration::zero())
                {
                    //
                    // note that forcing a test to run in under a certain amount of time is inherently fragile
                    // there's no guarantee that a thread, once started, actually gets `maxTestRunTime` nanoseconds of CPU

                    Time::TimeStamp testStart;

                    std::mutex m;
                    std::unique_lock<std::mutex> gate(m);

                    auto threadStarted = std::make_shared<std::condition_variable>();
                    std::thread timedRunner([&, threadStarted]()
                        {
                            m.lock();
                            m.unlock();

                            testStart = actualTest(false);
                            threadStarted->notify_all();
                        });
                    timedRunner.detach();

                    if (threadStarted->wait_for(gate, std::chrono::duration_cast<std::chrono::nanoseconds>(testTimeLimit)) == std::cv_status::timeout)
                    {
                        mImpl->OnTestFailure(test.TestDetails(), "Test failed to complete within " + std::to_string(Time::ToMilliseconds(testTimeLimit).count()) + " milliseconds.", LineInfo::empty());
                        mImpl->OnTestFinish(test.TestDetails(), testTimeLimit);
                        ++failedTests;
                    }
                    else
                    {
                        mImpl->OnTestFinish(test.TestDetails(), Time::ToDuration(Time::Clock::now() - testStart));
                    }
                }
                else
                {
                    actualTest(true);
                }
            }));
    }

    for (auto &test : futures)
    {
        test.get();
    }
    
    mImpl->OnAllTestsComplete((int)futures.size(), skippedTests, failedTests, Time::ToDuration(Time::Clock::now() - timeStart));

    return -failedTests;
}

}
