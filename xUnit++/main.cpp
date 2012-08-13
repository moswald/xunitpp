#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <tuple>
#include <vector>
#include "TestRunner.h"
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

    template<typename TFunc>
    void DoesNotThrow(TFunc &&fn)
    {
        try
        {
            fn();
        }
        catch (std::exception &e)
        {
            throw xUnitAssert(std::string("Caught std::exception with message: ") + e.what());
        }
        catch (...)
        {
            throw xUnitAssert(std::string("Caught unknown exception (crash) with message: ") + e.what());
        }
    }

    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn)
    {
        try
        {
            fn();
        }
        catch (TException e)
        {
            return e;
        }
        catch (std::exception &e)
        {
            throw xUnitAssert(std::string("Caught std::exception with message: ") + e.what());
        }
        catch (...)
        {
            throw xUnitAssert(std::string("Caught unknown exception (crash) with message: ") + e.what());
        }

        throw xUnitAssert(std::string("Did not catch any exceptions."));
    }

    void Fail()
    {
        throw xUnitAssert("Fail.");
    }

    void False(bool b)
    {
        if (b)
        {
            throw xUnitAssert("Expected false.");
        }
    }

    void True(bool b)
    {
        if (!b)
        {
            throw xUnitAssert("Expected true.");
        }
    }

    template<typename TContainer>
    void Empty(TContainer container)
    {
        if (!container.empty())
        {
            throw xUnitAssert("Container is not empty.");
        }
    }

    template<typename TContainer>
    void NotEmpty(TContainer container)
    {
        if (container.empty())
        {
            throw xUnitAssert("Container is empty.");
        }
    }

    template<typename TContainer, typename TItem>
    void DoesNotContain(TContainer container, TItem item)
    {
        if (std::find(container.begin(), container.end(), item))
        {
            throw xUnitAssert("Container contains item.");
        }
    }

    template<typename TContainer, typename TItem>
    void Contains(TContainer container, TItem item)
    {
        if (!std::find(container.begin(), container.end(), item))
        {
            throw xUnitAssert("Container does not contain item.");
        }
    }
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
