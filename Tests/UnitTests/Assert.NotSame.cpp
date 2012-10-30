#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertNotSame")
{

struct NotSameFixture
{
    NotSameFixture()
        : obj(2)
        , different(3)
    {
    }

    int obj;
    int different;
};

FACT_FIXTURE("NotSameSuccess", NotSameFixture)
{
    Assert.NotSame(obj, different);
}

FACT_FIXTURE("NotSameForPointersSuccess", NotSameFixture)
{
    Assert.NotSame(&obj, &different);
}

FACT_FIXTURE("NotSameAssertsOnFailure", NotSameFixture)
{
    Assert.Throws<xUnitAssert>([&]() { Assert.NotSame(obj, obj); });
}

FACT_FIXTURE("NotSameForPointersAssertsOnFailure", NotSameFixture)
{
    Assert.Throws<xUnitAssert>([&]() { Assert.NotSame(&obj, &obj); });
}

FACT_FIXTURE("NotSameAppendsUserMessage", NotSameFixture)
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.NotSame(obj, obj) << msg; });

    Assert.Contains(assert.UserMessage(), msg.c_str());
}

}