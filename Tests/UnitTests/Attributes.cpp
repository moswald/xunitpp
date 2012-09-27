#include "xUnit++/IOutput.h"
#include "xUnit++/TestCollection.h"
#include "xUnit++/xUnitTestRunner.h"
#include "xUnit++/xUnit++.h"
#include "Helpers/OutputRecord.h"

SUITE("Attributes")
{

ATTRIBUTES(("Cats", "Meow"))
{
FACT("TestWithAttributes")
{
    for (const auto &test : xUnitpp::TestCollection::Instance().Tests())
    {
        if (test->TestDetails().Name == "TestWithAttributes")
        {
            auto it = test->TestDetails().Attributes.find("Cats");
            Assert.True(it != test->TestDetails().Attributes.end());
            Assert.True(it->second == "Meow");
            return;
        }
    }

    Assert.Fail() << "Could not find self in test list.";
}
}

FACT("SkippedTestsShouldNotBeInstantiated")
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

    xUnitpp::Tests::OutputRecord record;

    xUnitpp::AttributeCollection attributes;
    attributes.insert(std::make_pair("Skip", "Testing skip."));

    xUnitpp::TestCollection collection;
    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> localEventRecorders;
    xUnitpp::TestCollection::Register reg(collection, []() { SkippedTest().RunTest(); },
        "SkippedTest", "Attributes", attributes, -1, __FILE__, __LINE__, localEventRecorders);

    xUnitpp::RunTests(record, [](const xUnitpp::TestDetails &) { return true; },
        collection.Tests(), xUnitpp::Time::Duration::zero(), 0);
}

}
