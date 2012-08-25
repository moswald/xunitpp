#include "Fact.h"
#include "TestCollection.h"
#include "xUnit++.h"

using xUnitpp::Assert;

SUITE(Attributes)
{

ATTRIBUTES(TestWithAttributes, ("Cats", "Meow"))
FACT(TestWithAttributes)
{
    for (const auto &fact : xUnitpp::TestCollection::Facts())
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

ATTRIBUTES(SkippedTest, ("Skip", "no reason"))
FACT(SkippedTest)
{
    Assert.Fail("Skipped tests should not be run.");
}

}
