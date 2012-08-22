#include <tuple>
#include <vector>
#include "xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

SUITE(AssertInRange)
{

std::vector<std::tuple<float, int, int>> InRangeSuccessData()
{
    std::vector<std::tuple<float, int, int>> data;

    data.push_back(std::make_tuple(0.0f, 0, 1));
    data.push_back(std::make_tuple(0.5f, 0, 1));
    data.push_back(std::make_tuple(0.0f, -1, 1));
    data.push_back(std::make_tuple(-1.0f, -1, 0));

    return data;
}

THEORY(InRangeSuccess, (float actual, int min, int max), InRangeSuccessData)
{
    Assert.InRange(actual, min, max);
}

std::vector<std::tuple<int, int, int>> InRangeAssertsOnFailureData()
{
    std::vector<std::tuple<int, int, int>> data;

    data.push_back(std::make_tuple(1, 0, 1));
    data.push_back(std::make_tuple(2, 0, 1));
    data.push_back(std::make_tuple(-1, 0, 1));
    data.push_back(std::make_tuple(-3, -2, 1));
    data.push_back(std::make_tuple(1, -2, 1));
    data.push_back(std::make_tuple(2, -2, 1));

    return data;
}

THEORY(InRangeAssertsOnFailure, (int actual, int min, int max), InRangeAssertsOnFailureData)
{
    Assert.Throws<xUnitAssert>([=]() { Assert.InRange(actual, min, max); });
}

FACT(InRangeAppendsUserMessage)
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([=]() { Assert.InRange(1, 0, 1, msg); });

    Assert.Contains(assert.what(), msg.c_str());
}

FACT(InRangeNeedsValidRange)
{
    Assert.Throws<std::invalid_argument>([]() { Assert.InRange(0, 0, 0); });
    Assert.Throws<std::invalid_argument>([]() { Assert.InRange(0, 1, 0); });
}

}