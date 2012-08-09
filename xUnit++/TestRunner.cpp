#include <atomic>
#include <future>
#include <limits>
#include <mutex>
#include <random>
#include <vector>
#include "TestRunner.h"
#include "TestCollection.h"

namespace xUnitpp
{

int RunAllTests(const std::string &suite, size_t maxTestRunTime, size_t maxConcurrent)
{
    return TestRunner().RunTests(TestCollection::Facts(), TestCollection::Theories(), suite, maxTestRunTime, maxConcurrent);
}

TestRunner::TestRunner(/*std::function<void(const TestDetails &)> onTestStart,
                       std::function<void(const TestDetails &, const std::string &)> onTestFailure,
                       std::function<void(const TestDetails &, const std::chrono::duration<float, std::milli>)> onTestFinish,
                       std::function<void(int, int, int, const std::chrono::duration<float, std::milli>)> onAllTestsComplete*/)
//    : mOnTestStart(onTestStart)
//    , mOnTestFailure(onTestFailure)
//    , mOnTestFinish(onTestFinish)
//    , mOnAllTestsComplete(onAllTestsComplete)
{
}

int TestRunner::RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite, size_t maxTestRunTime, size_t maxConcurrent)
{
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

    std::vector<TestInstance> allTests;
    size_t id = 0;
    size_t groupId = 0;

    for (auto &fact : facts)
    {
        if (suite == "" || fact.Suite() == suite)
        {
            allTests.emplace_back(TestInstance(++id, ++groupId, 1, fact.Test()));
        }
    }

    for (auto &theorySet : theories)
    {
        if (suite == "" || theorySet.Suite() == suite)
        {
            ++groupId;

        for (auto &theory : theorySet.Theories())
        {
                allTests.emplace_back(TestInstance(++id, groupId, theorySet.Theories().size(), theory));
            }
        }
    }

    std::shuffle(allTests.begin(), allTests.end(), std::default_random_engine(std::random_device()()));

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
    };

    std::vector<std::future<void>> futures;

    for (auto &test : allTests)
    {
        futures.push_back(std::async([&]()
            {
                CounterGuard guard(threadCounter);

                try
                {
                    test.test();
                }
                catch (std::exception &)
                {
                }
                }));
    }

    for (auto &test : futures)
    {
        test.get();
    }

    return 0;
}

}
