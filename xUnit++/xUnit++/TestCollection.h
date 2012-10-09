#ifndef TESTCOLLECTION_H_
#define TESTCOLLECTION_H_

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <vector>
#include "xUnitTest.h"

namespace xUnitpp
{

class TestEventRecorder;

// !!!VS convert this to an initializer list when VS implements them
template<typename TTuple>
static std::function<std::vector<TTuple>()> TheoryData(int count, TTuple tuples[])
{
    std::vector<TTuple> data;

    for (int i = 0; i != count; ++i)
    {
        data.push_back(tuples[i]);
    }

    return [=]() { return data; };
}

class Check;

class TestCollection
{
    friend class Register;

public:
    class Register
    {
        // !!!VS someday, Visual Studio will understand variadic templates
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
            const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line, const std::vector<std::shared_ptr<TestEventRecorder>> &testEventRecorders);

        template<typename TTheory, typename TTheoryData>
        Register(TestCollection &collection, TTheory theory, TTheoryData theoryData, const std::string &name, const std::string &suite,
            const AttributeCollection &attributes, int milliseconds, const std::string &filename, int line, const std::vector<std::shared_ptr<TestEventRecorder>> &testEventRecorders)
        {
            int id = 0;
            for (auto t : theoryData())
            {
                // !!! someday, I'd like to embed the actual parameter values, rather than the theory data index
                // not sure how feasible that is in C++, since it's lacking the type reflection of C# :(
                auto theoryName = name + "(" + std::to_string(++id) + ")";

                collection.mTests.push_back(std::make_shared<xUnitTest>(TheoryHelper(theory, std::move(t)), theoryName, suite,
                    attributes, Time::ToDuration(Time::ToMilliseconds(milliseconds)), filename, line, testEventRecorders));
            }
        }
    };

    static TestCollection &Instance();

    const std::vector<std::shared_ptr<xUnitTest>> &Tests();

private:
    std::vector<std::shared_ptr<xUnitTest>> mTests;
};

}

#endif
