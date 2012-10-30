#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertSame")
{

FACT("SameSuccess")
{
    int x;

    Assert.Same(x, x);
}

FACT("SameForPointersSuccess")
{
    int x;

    Assert.Same(&x, &x);
}

FACT("SameForConstPointersSuccess")
{
    int x;
    const int *px = &x;

    Assert.Same(px, px);
}

FACT("SameAssertsOnFailure")
{
    int x = 2;
    int y = 2;

    Assert.Throws<xUnitAssert>([=]() { Assert.Same(x, y); });
}

FACT("SameForPointersAssertsOnFailure")
{
    int x = 2;
    int y = 2;

    Assert.Throws<xUnitAssert>([=]() { Assert.Same(&x, &y); });
}

FACT("SameAppendsUserMessage")
{
    static const std::string msg = "xUnit++";

    int x = 0;
    int y = 0;

    auto assert = Assert.Throws<xUnitAssert>([=]() { Assert.Same(x, y) << msg; });

    Assert.Contains(assert.UserMessage(), msg.c_str());
}

}