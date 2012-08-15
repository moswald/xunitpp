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

const class
{
private:
    static double round(double value, size_t precision)
    {
        if (value < 0)
        {
            return std::ceil((value - 0.5) * std::pow(10, precision)) / std::pow(10, precision);
        }
        else
        {
            return std::floor((value + 0.5) * std::pow(10, precision)) / std::pow(10, precision);
        }
    }

public:
    template<typename T0, typename T1, typename TComparer>
    void Equal(T0 t0, T1 t1, TComparer comparer) const
    {
        if (!comparer(t0, t1))
        {
            throw xUnitAssert("Not equal.");
        }
    }

    template<typename T0, typename T1>
    void Equal(T0 t0, T1 t1) const
    {
        Equal(t0, t1, [](T0 t0, T1 t1) { return t0 == t1; });
    }

    void Equal(double expected, double actual, size_t precision) const
    {
        auto er = round(expected, precision);
        auto ar = round(actual, precision);

        Equal(er, ar, [](double er, double ar) { return er == ar; });
    }

    template<typename T, typename TComparer>
    void Equal(const std::vector<T> &expected, const std::vector<T> &actual, TComparer comparer) const
    {
        if (expected.size() != actual.size())
        {
            throw xUnitAssert("Lengths differ.");
        }

        for (auto e = expected.begin(), a = actual.begin(); e != expected.end(); ++e, ++a)
        {
            Equal(*e, *a, comparer);
        }
    }

    template<typename T>
    void Equal(const std::vector<T> &expected, const std::vector<T> &actual) const
    {
        Equal(expected, actual, [](const T &t0, const T &t1) { return t0 == t1; });
    }

    template<typename T0, typename T1, typename TComparer>
    void NotEqual(T0 t0, T1 t1, TComparer comparer) const
    {
        if (comparer(t0, t1))
        {
            throw xUnitAssert("Equal.");
        }
    }

    template<typename T0, typename T1>
    void NotEqual(T0 t0, T1 t1) const
    {
        NotEqual(t0, t1, [](T0 t0, T1 t1) { return t0 == t1; });
    }

    template<typename T, typename TComparer>
    void NotEqual(const std::vector<T> &expected, const std::vector<T> &actual, TComparer comparer)
    {
        if (expected.size() == actual.size())
        {
            for (auto e = expected.begin(), a = actual.begin(); e != expected.end(); ++e, ++a)
            {
                NotEqual(*e, *a, comparer);
            }
        }
    }

    template<typename T>
    void NotEqual(const std::vector<T> &expected, const std::vector<T> &actual)
    {
        NotEqual(expected, actual, [](const T &t0, const T &t1) { return t0 == t1; });
    }

    template<typename TFunc>
    void DoesNotThrow(TFunc &&fn) const
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
    TException Throws(TFunc &&fn) const
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

    void Fail(const std::string &msg) const
    {
        throw xUnitAssert(msg);
    }

    void False(bool b, const std::string &msg) const
    {
        if (b)
        {
            throw xUnitAssert(msg);
        }
    }

    void False(bool b) const
    {
        False(b, "Expected false.");
    }

    void True(bool b, const std::string &msg) const
    {
        if (!b)
        {
            throw xUnitAssert(msg);
        }
    }

    void True(bool b) const
    {
        True(b, "Expected true.");
    }

    template<typename TContainer>
    void Empty(TContainer container) const
    {
        if (!container.empty())
        {
            throw xUnitAssert("Container is not empty.");
        }
    }

    template<typename TContainer>
    void NotEmpty(TContainer container) const
    {
        if (container.empty())
        {
            throw xUnitAssert("Container is empty.");
        }
    }

    template<typename TContainer, typename TItem>
    void DoesNotContain(TContainer container, TItem item) const
    {
        if (std::find(container.begin(), container.end(), item))
        {
            throw xUnitAssert("Container contains item.");
        }
    }

    void DoesNotContain(const std::string &actualString, const std::string &expectedSubstring) const
    {
        if (actualString.find(expectedSubstring) != std::string::npos)
        {
            throw xUnitAssert("Actual string contains substring.");
        }
    }

    template<typename TContainer, typename TItem>
    void Contains(TContainer container, TItem item) const
    {
        if (!std::find(container.begin(), container.end(), item))
        {
            throw xUnitAssert("Container does not contain item.");
        }
    }

    void Contains(const std::string &actualString, const std::string &expectedSubstring) const
    {
        if (actualString.find(expectedSubstring) == std::string::npos)
        {
            throw xUnitAssert("Actual string does not contain substring.");
        }
    }

    template<typename TActual, typename TRange>
    void InRange(TActual actual, TRange min, TRange max) const
    {
        if (actual < min || actual >= max)
        {
            throw xUnitAssert("Value does not exist in range.");
        }
    }

    template<typename TActual, typename TRange>
    void NotInRange(TActual actual, TRange min, TRange max) const
    {
        if (actual >= min && actual < max)
        {
            throw xUnitAssert("Value does not exist in range.");
        }
    }

    template<typename T>
    void NotNull(T value) const
    {
        if (value == nullptr)
        {
            throw xUnitAssert("Value is null.");
        }
    }

    template<typename T>
    void Null(T value) const
    {
        if (value != nullptr)
        {
            throw xUnitAssert("Value is not null.");
        }
    }

    template<typename T>
    void NotSame(const T &t0, const T &t1) const
    {
        if (&t0 == &t1)
        {
            throw xUnitAssert("Values are the same instance.");
        }
    }

    template<typename T>
    void Same(const T &t0, const T &t1) const
    {
        if (&t0 != &t1)
        {
            throw xUnitAssert("Values are not the same instance.");
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
