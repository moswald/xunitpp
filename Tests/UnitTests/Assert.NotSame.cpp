#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

SUITE(AssertNotSame)
{

FACT(NotSameSuccess)
{
    int x = 2;
    int y = 2;

    Assert.NotSame(x, y);
}

FACT(NotSameForPointersSuccess)
{
    int x = 2;
    int y = 2;

    Assert.NotSame(&x, &y);
}

FACT(NotSameAssertsOnFailure)
{
    int x;

    Assert.Throws<xUnitAssert>([=]() { Assert.NotSame(x, x); });
}

FACT(NotSameForPointersAssertsOnFailure)
{
    int x;

    Assert.Throws<xUnitAssert>([=]() { Assert.NotSame(&x, &x); });
}

FACT(NotSameAppendsUserMessage)
{
    static const std::string msg = "xUnit++";

    int x;

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.NotSame(x, x) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

}