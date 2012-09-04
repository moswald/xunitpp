#include <chrono>
#include <string>
#include <tuple>
#include <vector>
#include "ExportApi.h"
#include "IOutput.h"
#include "TestCollection.h"
#include "xUnitTestRunner.h"
#include "xUnitTime.h"

namespace
{
    extern "C" __declspec(dllexport) void EnumerateTestDetails(xUnitpp::EnumerateTestDetailsCallback callback)
    {
        for (const auto &test : xUnitpp::TestCollection::Instance().Tests())
        {
            callback(test.TestDetails());
        }
    }

    extern "C" __declspec(dllexport) int FilteredTestsRunner(int timeLimit, xUnitpp::IOutput &testReporter, std::function<bool(const xUnitpp::TestDetails &)> filter)
    {
        return xUnitpp::TestRunner(testReporter).RunTests(filter,
            xUnitpp::TestCollection::Instance().Tests(), xUnitpp::Time::ToDuration(std::chrono::milliseconds(timeLimit)), 0);
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
    collection.mTests.emplace_back(xUnitTest(fn, name, suite, attributes, Time::ToDuration(std::chrono::milliseconds(milliseconds)), filename, line));
}

const std::vector<xUnitTest> &TestCollection::Tests()
{
    return mTests;
}

}
