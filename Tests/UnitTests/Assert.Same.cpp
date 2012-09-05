#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

SUITE(AssertSame)
{

FACT(SameSuccess)
{
    int x;

    Assert.Same(x, x);
}

FACT(SameForPointersSuccess)
{
    int x;

    Assert.Same(&x, &x);
}

FACT(SameForConstPointersSuccess)
{
    int x;
    const int *px = &x;

    Assert.Same(px, px);
}

FACT(SameAssertsOnFailure)
{
    int x = 2;
    int y = 2;

    Assert.Throws<xUnitAssert>([=]() { Assert.Same(x, y); });
}

FACT(SameForPointersAssertsOnFailure)
{
    int x = 2;
    int y = 2;

    Assert.Throws<xUnitAssert>([=]() { Assert.Same(&x, &y); });
}

FACT(SameAppendsUserMessage)
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.Same(0, 1, msg); });

    Assert.Contains(assert.what(), msg.c_str());
}

}