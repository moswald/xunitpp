#include <string>
#include <vector>
#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE(AssertContains)
{

FACT(ContainsForSequenceSuccess)
{
    std::vector<int> v(1, 0);

    Assert.Contains(v, 0);
}

FACT(ContainsForSequenceAssertsOnFailure)
{
    std::vector<int> v;

    Assert.Throws<xUnitAssert>([&]() { Assert.Contains(v, 0); });
}

FACT(ContainsForSequenceAppendsUserMessage)
{
    static const std::string msg = "xUnit++";
    std::vector<int> v;

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Contains(v, 0) << msg; });
    auto what = std::string(assert.what());

    Assert.NotEqual(std::string::npos, what.find(msg));
}

FACT(ContainsForStringSuccess)
{
    std::string actual = "abcd";

    Assert.Contains(actual, "a");
}

FACT(ContainsForStringAssertsOnFailure)
{
    std::string actual = "abcd";

    Assert.Throws<xUnitAssert>([&]() { Assert.Contains(actual, "xyz"); });
}

FACT(ContainsForStringAppendsUserMessage)
{
    static const std::string msg = "xUnit++";
    std::string actual = "abcd";

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Contains(actual, "xyz") << msg; });
    auto what = std::string(assert.what());

    Assert.NotEqual(std::string::npos, what.find(msg));
}

}
