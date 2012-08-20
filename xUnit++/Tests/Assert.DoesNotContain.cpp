#include <string>
#include <vector>
#include "../xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

SUITE(AssertDoesNotContain)
{

FACT(DoesNotContainForSequenceSuccess)
{
    std::vector<int> v;

    Assert.DoesNotContain(v, 0);
}

FACT(DoesNotContainForSequenceAssertsOnFailure)
{
    std::vector<int> v(1, 0);

    Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(v, 0); });
}

FACT(DoesNotContainForSequenceAppendsUserMessage)
{
    static const std::string msg = "xUnit++";
    std::vector<int> v(1, 0);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(v, 0, msg); });

    Assert.Contains(assert.what(), msg.c_str());
}

FACT(DoesNotContainForStringSuccess)
{
    std::string expected = "abcd";

    Assert.DoesNotContain(expected, "xyz");
}

FACT(DoesNotContainForStringAssertsOnFailure)
{
    std::string expected = "abcd";

    Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(expected, "bc"); });
}

FACT(DoesNotContainForStringAppendsUserMessage)
{
    static const std::string msg = "xUnit++";
    std::string expected = "abcd";

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(expected, "ab", msg); });

    Assert.Contains(assert.what(), msg.c_str());
}

}