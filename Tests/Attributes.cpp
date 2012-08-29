#include "Fact.h"
#include "IOutput.h"
#include "TestCollection.h"
#include "xUnitTestRunner.h"
#include "xUnit++.h"

using xUnitpp::Assert;

SUITE(Attributes)
{

ATTRIBUTES(TestWithAttributes, ("Cats", "Meow"))
FACT(TestWithAttributes)
{
    for (const auto &fact : xUnitpp::TestCollection::Instance().Facts())
    {
        if (fact.TestDetails().Name == "TestWithAttributes")
        {
            auto it = fact.TestDetails().Attributes.find("Cats");
            Assert.True(it != fact.TestDetails().Attributes.end());
            Assert.True(it->second == "Meow");
            return;
        }
    }

    Assert.Fail("Could not find self in test list.");
}

FACT(SkippedTestsShouldNotBeInstantiated)
{
    // have to set this internal test up manually since the macros don't work embedded within each other
    struct SkippedTest : xUnitpp::NoFixture
    {
        SkippedTest()
        {
            Assert.Fail("Should not be instantiated.");
        }
    
        void RunTest()
        {
            Assert.Fail("Should not be run.");
        }
    };

    struct EmptyReporter : xUnitpp::IOutput
    {
        virtual void ReportStart(const xUnitpp::TestDetails &, int) override
        {
        }

        virtual void ReportFailure(const xUnitpp::TestDetails &, int, const std::string &) override
        {
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &, const std::string &) override
        {
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &, int, xUnitpp::Duration) override
        {
        }

        virtual void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Duration) override 
        {
        }
    };

    xUnitpp::AttributeCollection attributes;
    attributes.insert(std::make_pair("Skip", "Testing skip."));

    xUnitpp::TestCollection collection;
    xUnitpp::TestCollection::Register reg(collection, []() { SkippedTest().RunTest(); }, "SkippedTest", "Attributes", attributes, -1, __FILE__, __LINE__);

    xUnitpp::TestRunner local(std::make_shared<EmptyReporter>());
    local.RunTests([](const xUnitpp::TestDetails &) { return true; },
        collection.Facts(), collection.Theories(), xUnitpp::Duration::zero(), 0);
}

}
