#include <tuple>
#include <vector>
#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertNotInRange")
{

std::vector<std::tuple<int, int, int>> NotInRangeSuccessData()
{
    std::vector<std::tuple<int, int, int>> data;

    data.push_back(std::make_tuple(-1, 0, 1));
    data.push_back(std::make_tuple(1, 0, 1));
    data.push_back(std::make_tuple(2, 0, 1));
    data.push_back(std::make_tuple(0, -2, -1));
    data.push_back(std::make_tuple(-1, -2, -1));
    data.push_back(std::make_tuple(-3, -2, -1));

    return data;
}

DATA_THEORY("NotInRangeSuccess", (int actual, int min, int max), NotInRangeSuccessData)
{
    Assert.NotInRange(actual, min, max);
}

std::vector<std::tuple<int, int, int>> NotInRangeAssertsOnFailureData()
{
    std::vector<std::tuple<int, int, int>> data;

    data.push_back(std::make_tuple(0, 0, 1));
    data.push_back(std::make_tuple(2, 1, 3));
    data.push_back(std::make_tuple(-1, -1, 0));
    data.push_back(std::make_tuple(-2, -3, -1));

    return data;
}

DATA_THEORY("NotInRangeAssertsOnFailure", (int actual, int min, int max), NotInRangeAssertsOnFailureData)
{
    Assert.Throws<xUnitAssert>([=]() { Assert.NotInRange(actual, min, max); });
}

FACT("NotInRangeAppendsUserMessage")
{
    static const std::string msg = "xUnit++";

    auto assert = Assert.Throws<xUnitAssert>([=]() { Assert.NotInRange(0, 0, 1) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

FACT("NotInRangeNeedsValidRange")
{
    Assert.Throws<std::invalid_argument>([]() { Assert.NotInRange(0, 0, 0); });
    Assert.Throws<std::invalid_argument>([]() { Assert.NotInRange(0, 1, 0); });
}

}