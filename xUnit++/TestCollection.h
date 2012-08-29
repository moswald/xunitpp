#ifndef TESTCOLLECTION_H_
#define TESTCOLLECTION_H_

#include <chrono>
#include <functional>
#include <map>
#include <vector>
#include "Theory.h"

namespace xUnitpp
{

class Fact;

class TestCollection
{
    friend class Register;

public:
    class Register
    {
        // !!!VS someday, Visual Studio will understand variadic macros
        // when it does, fix this collection

        // !!! should use a macro system to automate this
        template<typename TFn, typename TArg0>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0> &&t)
        {
            return [=]() { return theory(std::get<0>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1> &&t)
        {
            return [=]() { return theory(std::get<0>(t),
                                         std::get<1>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2> &&t)
        {
            return [=]() { return theory(std::get<0>(t),
                                         std::get<1>(t),
                                         std::get<2>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2, TArg3> &&t)
        {
            return [=]() { return theory(std::get<0>(t),
                                         std::get<1>(t),
                                         std::get<2>(t),
                                         std::get<3>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4> &&t)
        {
            return [=]() { return theory(std::get<0>(t),
                                         std::get<1>(t),
                                         std::get<2>(t),
                                         std::get<3>(t),
                                         std::get<4>(t)); };
        }

    public:
        Register(TestCollection &collection, const std::function<void()> &fn, const std::string &name, const std::string &suite,
            const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line);

        template<typename TTheory, typename TTheoryData>
        Register(TestCollection &collection, TTheory theory, TTheoryData theoryData, const std::string &name, const std::string &suite,
            const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line)
        {
            std::vector<std::function<void()>> theorySet;

            for (auto t : theoryData())
            {
                theorySet.emplace_back(TheoryHelper(theory, std::move(t)));
            }

            collection.mTheories.emplace_back(
                Theory(theorySet, name, suite, attributes, std::chrono::duration_cast<xUnitpp::Duration>(std::chrono::milliseconds(milliseconds)), filename, line));
        }
    };

    static TestCollection &Instance();

    const std::vector<Fact> &Facts();
    const std::vector<Theory> &Theories();

private:
    std::vector<Fact> mFacts;
    std::vector<Theory> mTheories;
};

}

#endif
