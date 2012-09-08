#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE(AssertNotEqual)
{

FACT(AssertNotEqualDefaultComparerWithSuccess)
{
    Assert.NotEqual(0, 1);
}

FACT(AssertNotEqualDefaultComparerAssertsOnFailure)
{
    Assert.Throws<xUnitAssert>([]() { Assert.NotEqual(0, 0); });
}

FACT(AssertNotEqualCustomComparerWithSuccess)
{
    Assert.NotEqual(0, 0, [](int, int) { return false; });
}

FACT(AssertNotEqualCustomComparerAssertsOnFailure)
{
    Assert.Throws<xUnitAssert>([]() { Assert.NotEqual(0, 1, [](int, int) { return true; }); });
}

FACT(AssertNotEqualAppendsUserMessage)
{
    static const std::string msg = "custom message";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.NotEqual(0, 0) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

FACT(AssertSequenceNotEqualDefaultComparerWithSuccess)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);
    v0.push_back(2);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    Assert.NotEqual(v0.begin(), v0.end(), v1.begin(), v1.end());
}

FACT(AssertSequenceNotEqualDefaultComparerAssertsOnFailure)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);
    v0.push_back(2);
    v0.push_back(3);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    Assert.Throws<xUnitAssert>([&]() { Assert.NotEqual(v0.begin(), v0.end(), v1.begin(), v1.end()); });
}

FACT(AssertSequenceNotEqualCustomComparerWithSuccess)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);
    v0.push_back(2);
    v0.push_back(3);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    Assert.NotEqual(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int, long long) { return false; });
}

FACT(AssertSequenceNotEqualDefaultAssertsOnFailureDueToMismatch)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);
    v0.push_back(2);
    v0.push_back(3);

    std::vector<long long> v1;
    v1.push_back(10);
    v1.push_back(11);
    v1.push_back(12);
    v1.push_back(13);

    Assert.Throws<xUnitAssert>([&]() { Assert.NotEqual(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int, long long) { return true; }); });
}

}
