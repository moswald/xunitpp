#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE(AssertFalse)
{

FACT(FalseSuccess)
{
    Assert.False(false);
}

FACT(FalseAssertsOnTrue)
{
    Assert.Throws<xUnitAssert>([]() { Assert.False(true); });
}

FACT(FalseAppendsCustomMessage)
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.False(true) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

}
