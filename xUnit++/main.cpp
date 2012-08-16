#include <exception>
#include <functional>
#include <iostream>
#include <cmath>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include "TestRunner.h"
#include "xUnitAssert.h"
#include "xUnitMacros.h"

using namespace xUnitpp;

FACT(SuccessfulFact)
{
    Assert.Equal(1, 1);
}

FACT(FailingFact)
{
    Assert.Equal(0, 1);
}

struct
{
    std::vector<std::tuple<int, char, bool>> operator()() const
    {
        auto result = std::vector<std::tuple<int, char, bool>>();
        result.push_back(std::make_tuple(1, (char)1, true));
        result.push_back(std::make_tuple(0, (char)1, false));
        result.push_back(std::make_tuple(0, (char)1, true));
        return result;
    }
} TestingTheoryDataProvider;

THEORY(TestingNewTheory, (int x, char y, bool equal), TestingTheoryDataProvider)
{
    if (equal)
    {
        Assert.Equal(x, y);
    }
    else
    {
        Assert.NotEqual(x, y);
    }
}

struct MyFixture
{
    MyFixture()
        : x(0)
        , y(1)
        , match(false)
    {
    }

    int x;
    int y;
    bool match;
};

FACT_FIXTURE(TestFactFixture, MyFixture)
{
    Assert.NotEqual(x, y);
}

SUITE(Special)
{
    FACT(SuiteFact)
    {
        Assert.Equal(0, 1);
    }

    std::vector<std::tuple<int, char>> SuiteTheoryDataProvider()
    {
        auto result = std::vector<std::tuple<int, char>>();
        result.push_back(std::make_tuple(1, (char)1));
        return result;
    }

    THEORY(SuiteTheory, (int x, char y), SuiteTheoryDataProvider)
    {
        Assert.Equal(x, y);
    }

    FACT_FIXTURE(SuiteFactFixture, MyFixture)
    {
        Assert.Equal(x, y - 1);
    }
}

FACT(LongRunning)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));
}

TIMED_FACT(LongRunningOk, 0)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TIMED_FACT(CustomLongRunning, 25)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(35));
}

int main()
{
    return xUnitpp::RunAllTests(std::chrono::milliseconds(50));
}
