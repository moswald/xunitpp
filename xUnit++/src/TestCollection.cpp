#include <chrono>
#include <memory>
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
            callback(test->TestDetails());
        }
    }

    extern "C" __declspec(dllexport) int FilteredTestsRunner(int timeLimit, int threadLimit, xUnitpp::IOutput &testReporter, std::function<bool(const xUnitpp::TestDetails &)> filter)
    {
        return xUnitpp::RunTests(testReporter, filter, xUnitpp::TestCollection::Instance().Tests(),
            xUnitpp::Time::ToDuration(xUnitpp::Time::ToMilliseconds(timeLimit)), threadLimit);
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
                                   const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line, std::shared_ptr<Check> check)
{
    collection.mTests.emplace_back(std::make_shared<xUnitTest>(fn, name, suite, attributes, Time::ToDuration(Time::ToMilliseconds(milliseconds)), filename, line, check));
}

const std::vector<std::shared_ptr<xUnitTest>> &TestCollection::Tests()
{
    return mTests;
}

}
