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

#include <iostream>

namespace
{

class SharedOutput : public xUnitpp::IOutput
{
public:
    SharedOutput(xUnitpp::IOutput &testReporter)
        : mOutput(testReporter)
    {
    }

    virtual void ReportStart(const xUnitpp::TestDetails &details) override
    {
        std::lock_guard<std::mutex> guard(mLock);
        mOutput.get().ReportStart(details);
    }

    virtual void ReportFailure(const xUnitpp::TestDetails &details, const std::string &message, const xUnitpp::LineInfo &lineInfo) override
    {
        std::lock_guard<std::mutex> guard(mLock);
        mOutput.get().ReportFailure(details, message, lineInfo);
    }

    virtual void ReportSkip(const xUnitpp::TestDetails &details, const std::string &reason) override
    {
        std::lock_guard<std::mutex> guard(mLock);
        mOutput.get().ReportSkip(details, reason);
    }

    virtual void ReportFinish(const xUnitpp::TestDetails &details, xUnitpp::Time::Duration time) override
    {
        std::lock_guard<std::mutex> guard(mLock);
        mOutput.get().ReportFinish(details, time);
    }

    virtual void ReportAllTestsComplete(size_t total, size_t skipped, size_t failed, xUnitpp::Time::Duration totalTime) override
    {
        mOutput.get().ReportAllTestsComplete(total, skipped, failed, totalTime);
    }

private:
    SharedOutput(const SharedOutput &);
    SharedOutput &operator =(SharedOutput);

private:
    std::mutex mLock;
    std::reference_wrapper<xUnitpp::IOutput> mOutput;
};

class AttachedOutput : public xUnitpp::IOutput
{
public:
    AttachedOutput(SharedOutput &output)
        : mAttached(true)
        , mOutput(std::ref(output))
    {
    }

    void Detach()
    {
        std::lock_guard<std::mutex> guard(mLock);
        mAttached = false;
    }

    virtual void ReportStart(const xUnitpp::TestDetails &details) override
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportStart(details);
        }
    }

    virtual void ReportFailure(const xUnitpp::TestDetails &details, const std::string &message, const xUnitpp::LineInfo &lineInfo) override
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportFailure(details, message, lineInfo);
        }
    }

    virtual void ReportSkip(const xUnitpp::TestDetails &details, const std::string &reason) override
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportSkip(details, reason);
        }
    }

    virtual void ReportFinish(const xUnitpp::TestDetails &details, xUnitpp::Time::Duration time) override
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportFinish(details, time);
        }
    }

    virtual void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Time::Duration) override
    {
        throw std::logic_error("No one holding an AttachedOutput object should be calling ReportAllTestsComplete.");
    }

private:
    AttachedOutput(const AttachedOutput &);
    AttachedOutput &operator =(AttachedOutput);

private:
    std::mutex mLock;
    bool mAttached;
    std::reference_wrapper<SharedOutput> mOutput;
};

}

namespace xUnitpp
{

int RunTests(IOutput &output, std::function<bool(const TestDetails &)> filter, const std::vector<std::shared_ptr<xUnitTest>> &tests, Time::Duration maxTestRunTime, size_t maxConcurrent)
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

    std::atomic<int> failedTests(0);
    int skippedTests = 0;

    SharedOutput sharedOutput(output);

    std::vector<std::shared_ptr<xUnitTest>> activeTests;
    std::copy_if(tests.begin(), tests.end(), std::back_inserter(activeTests), [&filter](const std::shared_ptr<xUnitTest> &test) { return filter(test->TestDetails()); });

    std::random_shuffle(activeTests.begin(), activeTests.end());

    std::vector<std::future<void>> futures;
    for (auto &test : activeTests)
    {
        {
            auto skip = test->TestDetails().Attributes.find("Skip");
            if (skip != test->TestDetails().Attributes.end())
            {
                skippedTests++;
                sharedOutput.ReportSkip(test->TestDetails(), skip->second);
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

                //
                // We are deliberately not capturing any values by reference, since the thread running this lambda may be detached
                // and abandoned by a timed test. If that were to happen, variables on the stack would get destroyed out from underneath us.
                // Instead, we're going to make copies that are guaranteed to outlive our method, and return the test status.
                // If the running thread is still valid, it can manage updating the count of failed threads if necessary.
                auto actualTest = [](bool reportEnd, std::shared_ptr<xUnitTest> runningTest, std::shared_ptr<AttachedOutput> output)-> std::tuple<Time::TimeStamp, bool>
                    {
                        bool failed = false;
                        Time::TimeStamp testStart;

                        auto CheckNonFatalErrors = [&]()
                        {
                            if (!failed && !runningTest->NonFatalFailures().empty())
                            {
                                failed = true;
                                for (auto &assert : runningTest->NonFatalFailures())
                                {
                                    output->ReportFailure(runningTest->TestDetails(), assert.what(), assert.LineInfo());
                                }
                            }
                        };

                        try
                        {
                            output->ReportStart(runningTest->TestDetails());

                            testStart = Time::Clock::now();
                            runningTest->Run();
                        }
                        catch (const xUnitAssert &e)
                        {
                            CheckNonFatalErrors();
                            output->ReportFailure(runningTest->TestDetails(), e.what(), e.LineInfo());
                            failed = true;
                        }
                        catch (const std::exception &e)
                        {
                            CheckNonFatalErrors();
                            output->ReportFailure(runningTest->TestDetails(), e.what(), LineInfo::empty());
                            failed = true;
                        }
                        catch (...)
                        {
                            CheckNonFatalErrors();
                            output->ReportFailure(runningTest->TestDetails(), "Unknown exception caught: test has crashed", LineInfo::empty());
                            failed = true;
                        }

                        CheckNonFatalErrors();

                        if (reportEnd)
                        {
                            output->ReportFinish(runningTest->TestDetails(), Time::ToDuration(Time::Clock::now() - testStart));
                        }

                        return std::make_tuple(testStart, failed);
                    };

                auto testTimeLimit = test->TestDetails().TimeLimit;
                if (testTimeLimit < Time::Duration::zero())
                {
                    testTimeLimit = maxTestRunTime;
                }

                if (testTimeLimit > Time::Duration::zero())
                {
                    //
                    // note that forcing a test to run in under a certain amount of time is inherently fragile
                    // there's no guarantee that a thread, once started, actually gets `maxTestRunTime` nanoseconds of CPU

                    auto m = std::make_shared<std::mutex>();
                    std::unique_lock<std::mutex> gate(*m);

                    auto attachedOutput = std::make_shared<AttachedOutput>(sharedOutput);
                    auto threadStarted = std::make_shared<std::condition_variable>();
                    auto testStart = std::make_shared<Time::TimeStamp>();
                    auto failed = std::make_shared<bool>();
                    std::thread timedRunner([=]()
                        {
                            m->lock();
                            m->unlock();

                            auto result = actualTest(false, test, attachedOutput);
                            *testStart = std::get<0>(result);
                            *failed = std::get<1>(result);

                            threadStarted->notify_all();
                        });
                    timedRunner.detach();

                    if (threadStarted->wait_for(gate, std::chrono::duration_cast<std::chrono::nanoseconds>(testTimeLimit)) == std::cv_status::timeout)
                    {
                        attachedOutput->Detach();
                        sharedOutput.ReportFailure(test->TestDetails(), "Test failed to complete within " + std::to_string(Time::ToMilliseconds(testTimeLimit).count()) + " milliseconds.", LineInfo::empty());
                        sharedOutput.ReportFinish(test->TestDetails(), testTimeLimit);
                        ++failedTests;
                    }
                    else
                    {
                        if (*failed)
                        {
                            ++failedTests;
                        }

                        sharedOutput.ReportFinish(test->TestDetails(), Time::ToDuration(Time::Clock::now() - *testStart));
                    }
                }
                else
                {
                    auto result = actualTest(true, test, std::make_shared<AttachedOutput>(sharedOutput));

                    if (std::get<1>(result))
                    {
                        ++failedTests;
                    }
                }
            }));
    }

    for (auto &test : futures)
    {
        test.get();
    }

    sharedOutput.ReportAllTestsComplete((int)futures.size(), skippedTests, failedTests, Time::ToDuration(Time::Clock::now() - timeStart));

    return failedTests;
}

}
