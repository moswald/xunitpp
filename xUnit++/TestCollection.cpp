#include <string>
#include <tuple>
#include <vector>
#include "TestCollection.h"
#include "Fact.h"

namespace
{
    extern "C" __declspec(dllexport) void ListAllTests(std::vector<std::tuple<std::string, xUnitpp::AttributeCollection>> &tests)
    {
        for (const auto &fact : xUnitpp::TestCollection::Facts())
        {
            tests.emplace_back(std::make_tuple(fact.TestDetails().Name, fact.TestDetails().Attributes));
        }

        for (const auto &theory : xUnitpp::TestCollection::Theories())
        {
            tests.emplace_back(std::make_tuple(theory.TestDetails().Name, theory.TestDetails().Attributes));
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

TestCollection::Register::Register(const std::function<void()> &fn, const std::string &name, const std::string &suite,
                                   const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line)
{
    TestCollection::Instance().mFacts.emplace_back(Fact(fn, name, suite, attributes, std::chrono::milliseconds(milliseconds), filename, line));
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
