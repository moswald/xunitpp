#include "TestCollection.h"
#include <cctype>
#include <chrono>
#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "ExportApi.h"
#include "IOutput.h"
#include "TestEventRecorder.h"
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

    extern "C" __declspec(dllexport) int FilteredTestsRunner(int timeLimit, int threadLimit, xUnitpp::IOutput &testReporter, xUnitpp::TestFilterCallback filter)
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

TestCollection::Register::Register(TestCollection &collection, std::function<void()> &&fn, std::string &&name, const std::string &suite,
            AttributeCollection &&attributes, int milliseconds, std::string &&filename, int line, std::vector<std::shared_ptr<TestEventRecorder>> &&testEventRecorders)
{
    collection.mTests.push_back(std::make_shared<xUnitTest>(std::move(fn), std::move(name), 0, "", suite, std::move(attributes), Time::ToDuration(Time::ToMilliseconds(milliseconds)), std::move(filename), line, std::move(testEventRecorders)));
}

const std::vector<std::shared_ptr<xUnitTest>> &TestCollection::Tests()
{
    return mTests;
}

std::deque<std::string> TestCollection::Register::SplitParams(std::string &&params)
{
    // hopefully simple rules:
    //
    // from right to left:
    // skip until alphanumeric
    // eat all alphanumerics (+ '_') as token
    // skip to next ',', or string begin
    // at string begin, we're done

    static const auto check = [](char c) { return std::isalnum(c) != 0 || c == '_'; };

    std::deque<std::string> paramList;

    if (!params.empty())
    {
        for (auto ite = params.end() - 1; ite != params.begin(); /* empty on purpose */)
        {
            while (ite != params.begin() && !check(*ite))
            {
                --ite;
            }

            auto itb = ite++;
            while (itb != params.begin() && check(*itb))
            {
                --itb;
            }

            paramList.emplace_front(itb + 1, ite);

            while (itb != params.begin() && *itb != ',')
            {
                --itb;
            }

            ite = itb;
        }
    }

    return paramList;
}

}
