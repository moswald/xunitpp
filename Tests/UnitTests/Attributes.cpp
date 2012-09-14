#include "xUnit++/IOutput.h"
#include "xUnit++/TestCollection.h"
#include "xUnit++/xUnitTestRunner.h"
#include "xUnit++/xUnit++.h"


SUITE(Attributes)
{

ATTRIBUTES(TestWithAttributes, ("Cats", "Meow"))
FACT(TestWithAttributes)
{
    for (const auto &test : xUnitpp::TestCollection::Instance().Tests())
    {
        if (test.TestDetails().Name == "TestWithAttributes")
        {
            auto it = test.TestDetails().Attributes.find("Cats");
            Assert.True(it != test.TestDetails().Attributes.end());
            Assert.True(it->second == "Meow");
            return;
        }
    }

    Assert.Fail() << "Could not find self in test list.";
}

FACT(SkippedTestsShouldNotBeInstantiated)
{
    // have to set this internal test up manually since the macros don't work embedded within each other
    struct SkippedTest : xUnitpp::NoFixture
    {
        SkippedTest()
        {
            Assert.Fail() << "Should not be instantiated.";
        }
    
        void RunTest()
        {
            Assert.Fail() << "Should not be run.";
        }
    };

    struct : xUnitpp::IOutput
    {
        virtual void ReportStart(const xUnitpp::TestDetails &) override
        {
        }

        virtual void ReportFailure(const xUnitpp::TestDetails &, const std::string &, const xUnitpp::LineInfo &) override
        {
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &, const std::string &) override
        {
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &, xUnitpp::Time::Duration) override
        {
        }

        virtual void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Time::Duration) override 
        {
        }
    } emptyReporter;

    xUnitpp::AttributeCollection attributes;
    attributes.insert(std::make_pair("Skip", "Testing skip."));

    xUnitpp::TestCollection collection;
    auto check = std::make_shared<xUnitpp::Check>();
    xUnitpp::TestCollection::Register reg(collection, []() { SkippedTest().RunTest(); },
        "SkippedTest", "Attributes", attributes, -1, __FILE__, __LINE__, check);

    xUnitpp::RunTests(emptyReporter, [](const xUnitpp::TestDetails &) { return true; },
        collection.Tests(), xUnitpp::Time::Duration::zero(), 0);
}

}
