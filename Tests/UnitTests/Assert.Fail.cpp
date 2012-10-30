#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertFail")
{

FACT("FailAlwaysAsserts")
{
    Assert.Throws<xUnitAssert>([]() { Assert.Fail(); });
}

FACT("FailAppendsMessage")
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.Fail() << msg; });

    Assert.Contains(assert.UserMessage(), msg.c_str());
}

}
