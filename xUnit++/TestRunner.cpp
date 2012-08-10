#include <atomic>
#include <future>
#include <limits>
#include <mutex>
#include <random>
#include <vector>
#include "TestRunner.h"
#include "TestCollection.h"
#include "xUnitAssert.h"

namespace
{

class ActiveTests
{
public:
    struct TestInstance
    {
        TestInstance(int id, int groupId, int groupSize, std::function<void()> test)
            : id(id)
            , groupId(groupId)
            , groupSize(groupSize)
            , test(test)
        {
        }

        size_t id;
        size_t groupId;
        size_t groupSize;

        std::function<void()> test;
    };

    ActiveTests(const std::vector<xUnitpp::Fact> &facts, const std::vector<xUnitpp::Theory> &theories, const std::string &suite)
    {
        size_t id = 0;
        size_t groupId = 0;

        for (auto &fact : facts)
        {
            if (suite == "" || fact.Suite() == suite)
            {
                mTests.emplace_back(TestInstance(++id, ++groupId, 1, fact.Test()));
            }
        }

        for (auto &theorySet : theories)
        {
            if (suite == "" || theorySet.Suite() == suite)
            {
                ++groupId;

                for (auto &theory : theorySet.Theories())
                {
                    mTests.emplace_back(TestInstance(++id, groupId, theorySet.Theories().size(), theory));
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

class TestRunner::Impl
{
public:
    Impl(std::function<void(const TestDetails &)> onTestStart,
         std::function<void(const TestDetails &, const std::string &)> onTestFailure,
         std::function<void(const TestDetails &, milliseconds)> onTestFinish,
         std::function<void(int, int, int, milliseconds)> onAllTestsComplete)
        : mOnTestStart(onTestStart)
        , mOnTestFailure(onTestFailure)
        , mOnTestFinish(onTestFinish)
        , mOnAllTestsComplete(onAllTestsComplete)
    {
    }

    void OnTestStart(const TestDetails &details)
    {
        std::lock_guard<std::mutex> guard(mStartMtx);
        OnTestStart(details);
    }

    void OnTestFailure(const TestDetails &details, const std::string &message)
    {
        std::lock_guard<std::mutex> guard(mFailureMtx);
        OnTestFailure(details, message);
    }

    void OnTestFinish(const TestDetails &details, milliseconds time)
    {
        std::lock_guard<std::mutex> guard(mFinishMtx);
        OnTestFinish(details, time);
    }


    void OnAllTestsComplete(int total, int skipped, int failed, milliseconds totalTime)
    {
        mOnAllTestsComplete(total, skipped, failed, totalTime);
    }

private:
    std::function<void(const TestDetails &)> mOnTestStart;
    std::function<void(const TestDetails &, const std::string &)> mOnTestFailure;
    std::function<void(const TestDetails &, milliseconds)> mOnTestFinish;
    std::function<void(int, int, int, milliseconds)> mOnAllTestsComplete;

    std::mutex mStartMtx;
    std::mutex mFailureMtx;
    std::mutex mFinishMtx;
};

size_t RunAllTests(const std::string &suite, size_t maxTestRunTime, size_t maxConcurrent)
{
    return
        TestRunner([](const TestDetails &) {},
                   [](const TestDetails &, const std::string &) {},
                   [](const TestDetails &, milliseconds) {},
                   [](int, int, int, milliseconds) {})
            .RunTests(TestCollection::Facts(), TestCollection::Theories(), suite, maxTestRunTime, maxConcurrent);
}

TestRunner::TestRunner(std::function<void(const TestDetails &)> onTestStart,
                       std::function<void(const TestDetails &, const std::string &)> onTestFailure,
                       std::function<void(const TestDetails &, milliseconds)> onTestFinish,
                       std::function<void(int, int, int, milliseconds)> onAllTestsComplete)
    : mImpl(new Impl(onTestStart, onTestFailure, onTestFinish, onAllTestsComplete))
{
}

size_t TestRunner::RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite, size_t maxTestRunTime, size_t maxConcurrent)
{
    auto timeStart = std::chrono::system_clock::now();

    ActiveTests activeTests(facts, theories, suite);

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
            std::lock_guard<std::mutex> guard(mtx);
            --activeThreads;
        }

    private:
        size_t maxThreads;
        size_t activeThreads;
        std::mutex mtx;
        std::condition_variable condition;
    } threadCounter(maxConcurrent);

    size_t failedTests = 0;

    std::vector<std::future<void>> futures;
    for (auto &test : activeTests)
    {
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
                    ThreadCounter &tc;
                } counterGuard(threadCounter);

                try
                {
                    test.test();
                }
                catch (xUnitAssert &assert)
                {
                }
                catch (std::exception &e)
                {
                }
                catch (...)
                {
                }
            }));
    }

    for (auto &test : futures)
    {
        test.get();
    }

    return failedTests;
}

}
