#include "xUnitTestRunner.h"
#include <atomic>
#include <chrono>
#include <future>
#include <limits>
#include <mutex>
#include <random>
#include <stdexcept>
#include <vector>
#include "Fact.h"
#include "ForceLinkModuleMacros.h"
#include "IOutput.h"
#include "TestCollection.h"
#include "TestDetails.h"
#include "xUnitAssert.h"
#include "xUnitTime.h"

ENABLE_MODULE_LINK(TestRunner)

namespace
{

extern "C" __declspec(dllexport) int FilteredTestsRunner(int timeLimit, xUnitpp::IOutput &testReporter, std::function<bool(const xUnitpp::TestDetails &)> filter)
{
    return xUnitpp::TestRunner(testReporter)
        .RunTests(filter, xUnitpp::TestCollection::Instance().Facts(), xUnitpp::TestCollection::Instance().Theories(),
                  std::chrono::duration_cast<xUnitpp::Duration>(std::chrono::milliseconds(timeLimit)), 0);
}

class ActiveTests
{
public:
    struct TestInstance
    {
        TestInstance(const xUnitpp::TestDetails &testDetails, int id, int groupId, int groupSize, std::function<void()> test)
            : testDetails(testDetails)
            , id(id)
            , dataIndex(-1)
            , groupId(groupId)
            , groupSize(groupSize)
            , test(test)
        {
        }

        TestInstance(const xUnitpp::TestDetails &testDetails, int id, int dataIndex, int groupId, int groupSize, std::function<void()> test)
            : testDetails(testDetails)
            , id(id)
            , dataIndex(dataIndex)
            , groupId(groupId)
            , groupSize(groupSize)
            , test(test)
        {
        }

        TestInstance(const TestInstance &other)
            : testDetails(other.testDetails)
            , id(other.id)
            , dataIndex(other.dataIndex)
            , groupId(other.groupId)
            , groupSize(other.groupSize)
            , test(other.test)
        {
        }

        TestInstance(TestInstance &&other)
        {
            swap(*this, other);
        }

        TestInstance &operator =(TestInstance other)
        {
            swap(*this, other);
            return *this;
        }

        friend void swap(TestInstance &ti0, TestInstance &ti1)
        {
            using std::swap;

            swap(ti0.testDetails, ti1.testDetails);
            swap(ti0.id, ti1.id);
            swap(ti0.dataIndex, ti1.dataIndex);
            swap(ti0.groupId, ti1.groupId);
            swap(ti0.groupSize, ti1.groupSize);
            swap(ti0.test, ti1.test);
        }

        xUnitpp::TestDetails testDetails;

        int id;
        int dataIndex;
        int groupId;
        int groupSize;

        std::function<void()> test;
    };

    ActiveTests(std::function<bool(const xUnitpp::TestDetails &)> filter, const std::vector<xUnitpp::Fact> &facts, const std::vector<xUnitpp::Theory> &theories)
    {
        size_t id = 0;
        size_t groupId = 0;

        for (auto &fact : facts)
        {
            if (filter(fact.TestDetails()))
            {
                mTests.emplace_back(TestInstance(fact.TestDetails(), ++id, ++groupId, 1, fact.Test()));
            }
        }

        for (auto &theorySet : theories)
        {
            if (filter(theorySet.TestDetails()))
            {
                ++groupId;
                size_t dataIndex = 0;
                for (auto &theory : theorySet.Theories())
                {
                    mTests.emplace_back(TestInstance(theorySet.TestDetails(), ++id, dataIndex++, groupId, theorySet.Theories().size(), theory));
                }
            }
        }

        std::shuffle(mTests.begin(), mTests.end(), std::default_random_engine(std::random_device()()));
    }

    std::vector<TestInstance>::iterator begin()
    {
        return mTests.begin();
    }

    std::vector<TestInstance>::iterator end()
    {
        return mTests.end();
    }

private:
    std::vector<TestInstance> mTests;
};

}

namespace xUnitpp
{

IOutput::~IOutput()
{
}

class TestRunner::Impl
{
public:
    Impl(IOutput &testReporter)
        : mTestReporter(testReporter)
    {
    }

    void OnTestStart(const TestDetails &details, int dataIndex)
    {
        std::lock_guard<std::mutex> guard(mStartMtx);
        mTestReporter.ReportStart(details, dataIndex);
    }

    void OnTestFailure(const TestDetails &details, int dataIndex, const std::string &message, const LineInfo &lineInfo)
    {
        std::lock_guard<std::mutex> guard(mFailureMtx);
        mTestReporter.ReportFailure(details, dataIndex, message, lineInfo);
    }

    void OnTestSkip(const TestDetails &details, const std::string &reason)
    {
        mTestReporter.ReportSkip(details, reason);
    }

    void OnTestFinish(const TestDetails &details, int dataIndex, xUnitpp::Duration time)
    {
        std::lock_guard<std::mutex> guard(mFinishMtx);
        mTestReporter.ReportFinish(details, dataIndex, time);
    }


    void OnAllTestsComplete(int total, int skipped, int failed, xUnitpp::Duration totalTime)
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

int TestRunner::RunTests(std::function<bool(const TestDetails &)> filter, const std::vector<Fact> &facts, const std::vector<Theory> &theories, xUnitpp::Duration maxTestRunTime, size_t maxConcurrent)
{
    auto timeStart = std::chrono::system_clock::now();

    ActiveTests activeTests(filter, facts, theories);

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

    std::vector<std::future<void>> futures;
    for (auto &test : activeTests)
    {
        {
            auto skip = test.testDetails.Attributes.find("Skip");
            if (skip != test.testDetails.Attributes.end())
            {
                skippedTests++;
                mImpl->OnTestSkip(test.testDetails, skip->second);
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

                auto actualTest = [&](bool reportEnd) -> TimeStamp
                    {
                        TimeStamp testStart;
                        try
                        {
                            mImpl->OnTestStart(test.testDetails, test.dataIndex);

                            testStart = Clock::now();
                            test.test();
                        }
                        catch (const xUnitAssert &e)
                        {
                            mImpl->OnTestFailure(test.testDetails, test.dataIndex, e.what(), e.LineInfo());
                        }
                        catch (const std::exception &e)
                        {
                            mImpl->OnTestFailure(test.testDetails, test.dataIndex, e.what(), LineInfo::empty());
                            ++failedTests;
                        }
                        catch (...)
                        {
                            mImpl->OnTestFailure(test.testDetails, test.dataIndex, "Unknown exception caught: test has crashed", LineInfo::empty());
                            ++failedTests;
                        }

                        if (reportEnd)
                        {
                            mImpl->OnTestFinish(test.testDetails, test.dataIndex, std::chrono::duration_cast<xUnitpp::Duration>(Clock::now() - testStart));
                        }

                        return testStart;
                    };

                auto testTimeLimit = test.testDetails.TimeLimit;
                if (testTimeLimit < xUnitpp::Duration::zero())
                {
                    testTimeLimit = maxTestRunTime;
                }

                if (testTimeLimit > xUnitpp::Duration::zero())
                {
                    //
                    // note that forcing a test to run in under a certain amount of time is inherently fragile
                    // there's no guarantee that a thread, once started, actually gets `maxTestRunTime` nanoseconds of CPU

                    TimeStamp testStart;

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
                        mImpl->OnTestFailure(test.testDetails, test.dataIndex, "Test failed to complete within " + std::to_string(ToMilliseconds(testTimeLimit).count()) + " milliseconds.", LineInfo::empty());
                        mImpl->OnTestFinish(test.testDetails, test.dataIndex, testTimeLimit);
                        ++failedTests;
                    }
                    else
                    {
                        mImpl->OnTestFinish(test.testDetails, test.dataIndex, std::chrono::duration_cast<xUnitpp::Duration>(Clock::now() - testStart));
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
    
    mImpl->OnAllTestsComplete(futures.size(), skippedTests, failedTests, std::chrono::duration_cast<xUnitpp::Duration>(Clock::now() - timeStart));

    return -failedTests;
}

}
