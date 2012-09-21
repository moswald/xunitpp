#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertTrue")
{

FACT("TrueSuccess")
{
    Assert.True(true);
}

FACT("TrueAssertsOnFalse")
{
    Assert.Throws<xUnitAssert>([]() { Assert.True(false); });
}

FACT("TrueAppendsCustomMessage")
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.True(false) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

}
