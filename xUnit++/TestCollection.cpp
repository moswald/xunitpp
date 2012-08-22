#include <string>
#include <vector>
#include "TestCollection.h"
#include "Fact.h"

namespace
{
    extern "C" __declspec(dllexport) void ListAllTests(std::vector<std::string> &tests)
    {
        for (const auto &fact : xUnitpp::TestCollection::Facts())
        {
            tests.push_back(fact.TestDetails().Name);
        }

        for (const auto &theory : xUnitpp::TestCollection::Theories())
        {
            tests.push_back(theory.TestDetails().Name);
        }
    }
}

namespace xUnitpp
{

TestCollection &TestCollection::Instance()
{
    static TestCollection collection;
    return collection;
}

TestCollection::Register::Register(const std::function<void()> &fn, const std::string &name, const std::string &suite, int milliseconds, const std::string &filename, int line)
{
    TestCollection::Instance().mFacts.emplace_back(Fact(fn, name, suite, std::chrono::milliseconds(milliseconds), filename, line));
}

const std::vector<Fact> &TestCollection::Facts()
{
    return Instance().mFacts;
}

const std::vector<Theory> &TestCollection::Theories()
{
    return Instance().mTheories;
}

}
