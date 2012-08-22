#include "xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

SUITE(AssertThrows)
{

FACT(ThrowsSuccess)
{
    struct exp
    {
        exp(int x)
            : x(x)
        {
        }

        int x;
    };

    auto x = Assert.Throws<exp>([]() { throw exp(10); });

    Assert.Equal(10, x.x);
}

FACT(ThrowsAssertsOnFailure)
{
    try
    {
        Assert.Throws<int>([=]() { });
    }
    catch(const xUnitAssert &)
    {
        return;
    }

    Assert.Fail();
}

FACT(ThrowAppendsMessages)
{
    static const std::string userMessage = "xUnit++";

    try
    {
        Assert.Throws<int>([=]() { }, userMessage);
    }
    catch(const xUnitAssert &assert)
    {
        Assert.Contains(assert.what(), userMessage.c_str());
        return;
    }

    Assert.Fail();
}

}
