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

    virtual void ReportEvent(const xUnitpp::TestDetails &details, const xUnitpp::TestEvent &evt) override
    {
        std::lock_guard<std::mutex> guard(mLock);
        mOutput.get().ReportEvent(details, evt);
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

class AttachedOutput
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

    void ReportStart(const xUnitpp::TestDetails &details)
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportStart(details);
        }
    }

    void ReportEvent(const xUnitpp::TestDetails &details, const xUnitpp::TestEvent &evt)
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportEvent(details, evt);
        }
    }

    void ReportSkip(const xUnitpp::TestDetails &details, const std::string &reason)
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportSkip(details, reason);
        }
    }

    void ReportFinish(const xUnitpp::TestDetails &details, xUnitpp::Time::Duration time)
    {
        std::lock_guard<std::mutex> guard(mLock);

        if (mAttached)
        {
            mOutput.get().ReportFinish(details, time);
        }
    }

    void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Time::Duration)
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
    auto timeStart = Time::Clock::now();

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
            auto skip = test->TestDetails().Attributes.Skipped();
            if (skip.first)
            {
                skippedTests++;
                sharedOutput.ReportSkip(test->TestDetails(), skip.second);
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
                auto actualTest = [](std::shared_ptr<xUnitTest> runningTest, std::shared_ptr<AttachedOutput> output) -> TestResult
                    {
                        output->ReportStart(runningTest->TestDetails());

                        auto result = runningTest->Run();

                        for (auto &event : runningTest->TestEvents())
                        {
                            output->ReportEvent(runningTest->TestDetails(), event);
                        }

                        return result;
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
                    auto testResult = std::make_shared<TestResult>();
                    std::thread timedRunner([=]()
                        {
                            m->lock();
                            m->unlock();

                            *testResult = actualTest(test, attachedOutput);

                            threadStarted->notify_all();
                        });
                    timedRunner.detach();

                    if (threadStarted->wait_for(gate, std::chrono::duration_cast<std::chrono::nanoseconds>(testTimeLimit)) == std::cv_status::timeout)
                    {
                        attachedOutput->Detach();
                        sharedOutput.ReportEvent(test->TestDetails(), TestEvent(EventLevel::Fatal, "Test failed to complete within " + ToString(Time::ToMilliseconds(testTimeLimit).count()) + " milliseconds."));
                        sharedOutput.ReportFinish(test->TestDetails(), testTimeLimit);
                        ++failedTests;
                    }
                    else
                    {
                        sharedOutput.ReportFinish(test->TestDetails(), test->Duration());

                        if (*testResult == TestResult::Failure)
                        {
                            ++failedTests;
                        }
                    }
                }
                else
                {
                    auto result = actualTest(test, std::make_shared<AttachedOutput>(sharedOutput));

                    sharedOutput.ReportFinish(test->TestDetails(), test->Duration());

                    if (result == TestResult::Failure)
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
