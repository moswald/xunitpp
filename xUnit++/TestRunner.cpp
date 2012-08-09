#include <atomic>
#include <future>
#include <vector>
#include "TestRunner.h"
#include "TestCollection.h"

namespace xUnitpp
{

int RunAllTests(const std::string &suite, size_t maxTestRunTime, size_t maxConcurrent)
{
    return TestRunner().RunTests(TestCollection::Facts(), TestCollection::Theories(), suite, maxTestRunTime, maxConcurrent);
}

int TestRunner::RunTests(const std::vector<Fact> &allFacts, const std::vector<Theory> &allTheories, const std::string &suite, size_t maxTestRunTime, size_t maxConcurrent)
{
    std::vector<Fact> facts;
    std::vector<Theory> theories;

    for (auto &fact : allFacts)
    {
        if (suite == "" || fact.Suite() == suite)
        {
            facts.emplace_back(fact);
        }
    }

    for (auto &theory : allTheories)
    {
        if (suite == "" || theory.Suite() == suite)
        {
            theories.emplace_back(theory);
        }
    }

    // !!! figure out how to get fully randomized test ordering, but sequential test output

    auto policy = std::launch::async;
    std::vector<std::future<void>> futures;

    size_t activeThreads(0);
    size_t id = 0;
    
    for (auto &fact : facts)
    {
        futures.push_back(std::async(policy, [&, maxConcurrent, id]()
            {
                fact.Run();
            }));

        ++id;
    }

    for (auto &theorySet : theories)
    {
        for (auto &theory : theorySet.Theories())
        {
            futures.push_back(std::async(policy, [&, maxConcurrent, id]()
                {
                    theorySet.Run(theory);
                }));

            ++id;
        }
    }

    return 0;
}

}
