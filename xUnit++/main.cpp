#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "xUnitAssert.h"
#include "xUnitMacros.h"

using namespace xUnitpp;

const class
{
public:
    template<typename T0, typename T1>
    void Equal(T0 t0, T1 t1) const
    {
        if (t0 != t1)
        {
            throw xUnitAssert("");
        }
    }

    template<typename T0, typename T1>
    void NotEqual(T0 t0, T1 t1) const
    {
        if (t0 == t1)
        {
            throw xUnitAssert("");
        }
    }

    // DoesNotThrow
    // Throws
    // Fail
    // False
    // True
    // Empty
    // NotEmpty
    // Contains
    // DoesNotContain


} Assert;

FACT(SuccessfulFact)
{
    Assert.Equal(1, 1);
}

FACT(FailingFact)
{
    Assert.Equal(0, 1);
}

void TestingTheory(int x, char y, bool equal)
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

THEORY(TestingTheory, int, char, bool)
{
    auto result = std::vector<std::tuple<int, char, bool>>();
    result.push_back(std::make_tuple(1, (char)1, true));
    result.push_back(std::make_tuple(0, (char)1, false));
    result.push_back(std::make_tuple(0, (char)1, true));
    return result;
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

    void SuiteTheory(int x, char y)
    {
        Assert.Equal(x, y);
    }

    THEORY(SuiteTheory, int, char)
    {
        auto result = std::vector<std::tuple<int, char>>();
        result.push_back(std::make_tuple(1, (char)1));
        return result;
    }

    FACT_FIXTURE(SuiteFactFixture, MyFixture)
    {
        Assert.Equal(x, y - 1);
    }
}

int main()
{
    xUnitpp::TestCollection::RunAllTests();
}
