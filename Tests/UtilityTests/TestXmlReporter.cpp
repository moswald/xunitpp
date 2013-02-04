#include "xUnit++/xUnit++.h"
#include "xUnit++/xUnitTestRunner.h"
#include "XmlReporter.h"
#include "tinyxml2.h"
#include "Helpers/TestFactory.h"

using xUnitpp::Utilities::XmlReporter;
using xUnitpp::Tests::TestFactory;

namespace
{
    namespace Filter
    {
        bool AllTests(const xUnitpp::ITestDetails &) { return true; }
        bool NoTests(const xUnitpp::ITestDetails &) { return false; }
    }

    class LocalTester
    {
    public:
        void Register(std::shared_ptr<xUnitpp::xUnitTest> test)
        {
            tests.push_back(test);
        }

        void Run(xUnitpp::IOutput &reporter)
        {
            xUnitpp::RunTests(reporter, &Filter::AllTests, tests, xUnitpp::Time::Duration::zero(), 0);
        }

    private:
        std::vector<std::shared_ptr<xUnitpp::xUnitTest>> tests;
    };
}

SUITE("XmlReporter")
{

FACT("XmlReporter generates valid xml with no tests")
{
    std::stringstream out;

    XmlReporter reporter(out);
    reporter.ReportAllTestsComplete(0, 0, 0, 0);

    Assert.Equal(tinyxml2::XMLError::XML_SUCCESS, tinyxml2::XMLDocument().Parse(out.str().c_str()));
}

// G++ doesn't like a tuple consisting of just a std::function<>
// the extra int is just a dummy parameter to make it compile
DATA_THEORY("XmlReporter generates valid xml after running tests", (std::function<void ()> test, int),
    ([]() -> std::vector<std::tuple<std::function<void()>, int>>
    {
        std::vector<std::tuple<std::function<void()>, int>> tests;

        tests.emplace_back([]() { Assert.True(true); }, 0);
        tests.emplace_back([]() { Assert.True(false); }, 0);
        tests.emplace_back(
            []()
            {
                Assert.True(true);
                Assert.True(false);
            }, 0);

        return tests;
    })
)
{
    std::stringstream out;

    XmlReporter reporter(out);

    {
        LocalTester local;
        local.Register(TestFactory(test).Name("Success"));

        local.Run(reporter);
    }

    Assert.Equal(tinyxml2::XMLError::XML_SUCCESS, tinyxml2::XMLDocument().Parse(out.str().c_str()));
}

}
