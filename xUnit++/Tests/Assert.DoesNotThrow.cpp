#include "../xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

SUITE(AssertDoesNotThrow)
{

FACT(DoesNotThrowSuccess)
{
    try
    {
        Assert.DoesNotThrow([]() { });
    }
    catch(...)
    {
        Assert.Fail();
    }
}

FACT(DoesNotThrowAssertsOnFailure)
{
    static const std::string msg = "xUnit++";

    try
    {
        Assert.DoesNotThrow([=]() { throw std::exception(msg.c_str()); });
    }
    catch(const xUnitAssert &)
    {
        return;
    }

    Assert.Fail();
}

FACT(DoesNotThrowAppendsMessages)
{
    static const std::string exceptionMessage = "xUnit++";
    static const std::string userMessage = "custom";

    try
    {
        Assert.DoesNotThrow([=]() { throw std::exception(exceptionMessage.c_str()); }, userMessage);
    }
    catch(const xUnitAssert &assert)
    {
        Assert.Contains(assert.what(), exceptionMessage.c_str());
        Assert.Contains(assert.what(), userMessage.c_str());
        return;
    }

    Assert.Fail();
}

}
