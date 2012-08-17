#include "../xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

FACT(AssertEqualWithDefaultComparerWithSuccess)
{
	Assert.Equal(0, 0);
}

FACT(AssertEqualWithDefaultComparerAssertsOnFailure)
{
    Assert.Throws<xUnitAssert>([]() { Assert.Equal(0, 1); });
}

FACT(AssertEqualWithCustomComparerWithSuccess)
{
    Assert.Equal(0, 1, [](int a, int b) { return true; });
}

FACT(AssertEqualWithCustomComparerAssertsOnFailure)
{
    Assert.Throws<xUnitAssert>([]() { Assert.Equal(0, 0, [](int a, int b) { return false; }); });
}

FACT(AssertEqualAppendsUserMessage)
{
    static const std::string msg = "custom message";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.Equal(0, 1, msg); });

    Assert.Contains(assert.what(), msg.c_str());
}

FACT(AssertEqualForFloatsWithinPrecision)
{
    auto f0 = 1.2345678f;
    auto f1 = 1.2349999f;

    Assert.Equal(f0, f1, 3);
}

FACT(AssertEqualForFloatsAssertsOnFailure)
{
    auto f0 = 1.2345678f;
    auto f1 = 1.2349999f;

    Assert.Throws<xUnitAssert>([=]() { Assert.Equal(f0, f1, 4); });
}

FACT(AssertSequenceEqualDefaultComparerWithSuccess)
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

    Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end());
}

FACT(AssertSequenceEqualDefaultComparerAssertsOnFailureDueToLength)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end()); });

    Assert.Contains(assert.what(), "at location 2");
}

FACT(AssertSequenceEqualDefaultComparerAssertsOnFailureDueToMismatch)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(0);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end()); });

    Assert.Contains(assert.what(), "at location 1");
}

FACT(AssertSequenceEqualCustomComparerWithSuccess)
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

    Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int a, long long b) { return true; });
}

FACT(AssertSequenceEqualCustomComparerAssertsOnFailureDueToLength)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int a, long long b) { return true; }); });

    Assert.Contains(assert.what(), "at location 2");
}

FACT(AssertSequenceEqualDefaultAssertsOnFailureDueToMismatch)
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int a, long long b) { return false; }); });

    Assert.Contains(assert.what(), "at location 0");
}
