#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
//#include "TupleHelpers.h"

class xUnitAssert : std::exception
{
public:
    xUnitAssert(const std::string &)
    {
    }
};

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

} Assert;

class xUnit
{
    friend class Register;
public:
    class Register
    {
        // !!! someday, Visual Studio will understand variadic macros
        // when it does, fix this collection

        // !!! should use a macro system to automate this
        template<typename TFn, typename TArg0>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0> &&t)
        {
            return [&, theory]() { return theory(std::forward<TArg0>(std::get<0>(t))); };
        }

        template<typename TFn, typename TArg0, typename TArg1>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1> &&t)
        {
            return [&, theory]() { return theory(std::forward<TArg0>(std::get<0>(t)),
                                         std::forward<TArg1>(std::get<1>(t))); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2> &&t)
        {
            return [&, theory]() { return theory(std::forward<TArg0>(std::get<0>(t)),
                                         std::forward<TArg1>(std::get<1>(t)),
                                         std::forward<TArg2>(std::get<2>(t))); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2, TArg3> &&t)
        {
            return [&, theory]() { return theory(std::forward<TArg0>(std::get<0>(t)),
                                        std::forward<TArg1>(std::get<1>(t)),
                                        std::forward<TArg2>(std::get<2>(t)),
                                        std::forward<TArg3>(std::get<3>(t))); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4> &&t)
        {
            return [&, theory]() { return theory(std::forward<TArg0>(std::get<0>(t)),
                                        std::forward<TArg1>(std::get<1>(t)),
                                        std::forward<TArg2>(std::get<2>(t)),
                                        std::forward<TArg3>(std::get<3>(t)),
                                        std::forward<TArg4>(std::get<4>(t))); };
        }

    public:
        Register(const std::function<void()> &fn)
        {
            xUnit::Instance().mFacts.push_back(fn);
        }

        template<typename TTheory, typename TTheoryData>
        Register(TTheory theory, TTheoryData theoryData)
        {
            std::vector<std::function<void()>> theorySet;

            for (auto t : theoryData())
            {
                theorySet.push_back(TheoryHelper(theory, std::move(t)));
            }
        
            xUnit::Instance().mTheories.push_back(theorySet);
        }
    };

    static void RunAllTests()
    {
        auto facts = 1;
        auto theories = 1;

        // !!! randomize
        for (auto fact : Instance().mFacts)
        {
            // !!! test runner should be single function
            // to avoid duplicated code
            try
            {
                std::cout << "fact " << facts++ << " of " << Instance().mFacts.size() << std::endl;
                fact();
            }
            catch (xUnitAssert &)
            {
                std::cout << "Caught xUnitAssert." << std::endl;
            }
        }

        // !!! randomize
        for (auto theorySet : Instance().mTheories)
        {
            std::cout << "theory " << theories++ << " of " << Instance().mTheories.size() << std::endl;

            auto instance = 1;

            for (auto theory : theorySet)
            {
                try
                {
                    std::cout << "theory instance " << instance++ << " of " << theorySet.size() << std::endl;
                    theory();
                }
                catch (xUnitAssert &)
                {
                    std::cout << "Caught xUnitAssert." << std::endl;
                } 
            }
        }
    }

private:
    static xUnit &Instance()
    {
        static xUnit instance;
        return instance;
    }

    std::vector<std::function<void()>> mFacts;
    std::vector<std::vector<std::function<void()>>> mTheories;
};

#define FACT(fn) \
    void fn(); \
    namespace { namespace fn ## _ns { xUnit::Register reg(&fn); } } \
    void fn()

FACT(FactOk)
{
    Assert.Equal(1, 1);
}

FACT(FactFails)
{
    Assert.Equal(0, 1);
}

#define THEORY_DATA(theory, ...) \
    void theory(__VA_ARGS__); \
    std::vector<std::tuple<__VA_ARGS__>> theory ## _data(); \
    namespace { namespace theory ## _ns { xUnit::Register reg(&theory, &theory ## _data); } } \
    std::vector<std::tuple<__VA_ARGS__>> theory ## _data()

//void TheoryWorks(int, char, bool);
//std::vector<std::tuple<int, char, bool>> DataGen();
//namespace { namespace TheoryWorks_ns { xUnit::Register reg(&TheoryWorks, &DataGen); } }
//

void Theory(int x, char y, bool equal)
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


THEORY_DATA(Theory, int, char, bool)
//std::vector<std::tuple<int, char, bool>> DataGen()
{
    auto result = std::vector<std::tuple<int, char, bool>>();
    result.push_back(std::make_tuple(1, (char)1, true));
    result.push_back(std::make_tuple(0, (char)1, false));
    result.push_back(std::make_tuple(0, (char)1, true));
    return result;
}

int main()
{
    xUnit::RunAllTests();
}
