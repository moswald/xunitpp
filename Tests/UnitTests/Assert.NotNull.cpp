#include <memory>
#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE(AssertNotNull)
{

FACT(NotNullForRawPointerSuccess)
{
    int x;

    Assert.NotNull(&x);
}

FACT(NotNullForDoubleConstRawPointerSuccess)
{
    int x;
    const int * const px = &x;

    Assert.NotNull(px);
}

FACT(NotNullForSmartPointerSuccess)
{
    std::unique_ptr<int> x(new int);

    Assert.NotNull(x);
}

FACT(NotNullForRawPointerAssertsOnFailure)
{
    Assert.Throws<xUnitAssert>([]() { Assert.NotNull(nullptr); });
}

FACT(NotNullForSmartPointerAssertsOnFailure)
{
    Assert.Throws<xUnitAssert>([]() { Assert.NotNull(std::unique_ptr<int>()); });
}

FACT(NotNullAppendsUserMessage)
{
    static const std::string msg = "xUnit++";
    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.NotNull(nullptr) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

}
