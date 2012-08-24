#include <string>
#include <tuple>
#include <vector>
#include "Fact.h"
#include "IOutput.h"
#include "TestCollection.h"
#include "xUnitTestRunner.h"

namespace
{
    extern "C" __declspec(dllexport) void ListAllTests(std::vector<xUnitpp::TestDetails> &tests)
    {
        for (const auto &fact : xUnitpp::TestCollection::Facts())
        {
            tests.push_back(fact.TestDetails());
        }

        for (const auto &theory : xUnitpp::TestCollection::Theories())
        {
            tests.push_back(theory.TestDetails());
        }
    }

    extern "C" __declspec(dllexport) int FilteredTestsRunner(int timeLimit, std::shared_ptr<xUnitpp::IOutput> testReporter, std::function<bool(const xUnitpp::TestDetails &)> filter)
    {
        return xUnitpp::RunFilteredTests(timeLimit, testReporter, filter);
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
