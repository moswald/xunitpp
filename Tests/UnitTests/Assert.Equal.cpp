#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertEqual")
{

FACT("AssertEqualWithDefaultComparerWithSuccess")
{
    Assert.Equal(0, 0);
}

FACT("AssertEqualWithDefaultComparerAssertsOnFailure")
{
    Assert.Throws<xUnitAssert>([]() { Assert.Equal(0, 1); });
}

FACT("AssertEqualWithCustomComparerWithSuccess")
{
    Assert.Equal(0, 1, [](int, int) { return true; });
}

FACT("AssertEqualWithCustomComparerAssertsOnFailure")
{
    Assert.Throws<xUnitAssert>([]() { Assert.Equal(0, 0, [](int, int) { return false; }); });
}

FACT("AssertEqualAppendsUserMessage")
{
    static const std::string msg = "custom message";

    auto assert = Assert.Throws<xUnitAssert>([]() { Assert.Equal(0, 1) << msg; });

    Assert.Contains(assert.UserMessage(), msg.c_str());
}

FACT("AssertEqualForFloatsWithinPrecision")
{
    auto f0 = 1.2345678f;
    auto f1 = 1.2349999f;

    Assert.Equal(f0, f1, 3);
}

FACT("AssertEqualForFloatsAssertsOnFailure")
{
    auto f0 = 1.2345678f;
    auto f1 = 1.2349999f;

    Assert.Throws<xUnitAssert>([=]() { Assert.Equal(f0, f1, 4); });
}

FACT("AssertSequenceEqualDefaultComparerWithSuccess")
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

FACT("SequenceEqual should assert with differing lengths")
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

    Assert.Contains(assert.CustomMessage(), "at location 2");
}

FACT("SequenceEqual should assert due to element mismatch")
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

    Assert.Contains(assert.CustomMessage(), "at location 1");
}

FACT("AssertSequenceEqualCustomComparerWithSuccess")
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

    Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int, long long) { return true; });
}

FACT("SequenceEqual should assert with differing lengths (custom comparer)")
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int, long long) { return true; }); });

    Assert.Contains(assert.CustomMessage(), "at location 2");
}

FACT("SequenceEqual should assert due to element mismatch (custom comparer)")
{
    std::vector<int> v0;
    v0.push_back(0);
    v0.push_back(1);

    std::vector<long long> v1;
    v1.push_back(0);
    v1.push_back(1);
    v1.push_back(2);
    v1.push_back(3);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Equal(v0.begin(), v0.end(), v1.begin(), v1.end(), [](int, long long) { return false; }); });

    Assert.Contains(assert.CustomMessage(), "at location 0");
}

FACT("EqualForStrings")
{
    std::string expected = "abcd";
    std::string actual = "abc";

    Assert.Throws<xUnitAssert>([=]() { Assert.Equal(expected.c_str(), actual.c_str()); });
    Assert.Throws<xUnitAssert>([=]() { Assert.Equal(expected.c_str(), actual); });
    Assert.Throws<xUnitAssert>([=]() { Assert.Equal(expected, actual.c_str()); });
    Assert.Throws<xUnitAssert>([=]() { Assert.Equal(expected, actual); });
}

FACT("const char * should be equal to char *")
{
    char actual[] = "hi";
    Check.Equal("hi", actual);
}

}
