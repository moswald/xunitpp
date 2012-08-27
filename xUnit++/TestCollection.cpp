#include <string>
#include <tuple>
#include <vector>
#include "Fact.h"
#include "ForceLinkModuleMacros.h"
#include "IOutput.h"
#include "TestCollection.h"
#include "xUnitTestRunner.h"

LINK_MODULE(TestRunner)

namespace
{
    extern "C" __declspec(dllexport) void ListAllTests(std::vector<xUnitpp::TestDetails> &tests)
    {
        for (const auto &fact : xUnitpp::TestCollection::Instance().Facts())
        {
            tests.push_back(fact.TestDetails());
        }

        for (const auto &theory : xUnitpp::TestCollection::Instance().Theories())
        {
            tests.push_back(theory.TestDetails());
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

TestCollection::Register::Register(TestCollection &collection, const std::function<void()> &fn, const std::string &name, const std::string &suite,
                                   const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line)
{
    collection.mFacts.emplace_back(Fact(fn, name, suite, attributes, std::chrono::milliseconds(milliseconds), filename, line));
}

const std::vector<Fact> &TestCollection::Facts()
{
    return mFacts;
}

const std::vector<Theory> &TestCollection::Theories()
{
    return mTheories;
}

}
