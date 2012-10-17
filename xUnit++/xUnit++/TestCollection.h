#ifndef TESTCOLLECTION_H_
#define TESTCOLLECTION_H_

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <deque>
#include <vector>
#include "xUnitTest.h"
#include "xUnitToString.h"

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
            return [=]() { theory(std::get<0>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1> &&t)
        {
            return [=]() { theory(std::get<0>(t),
                                  std::get<1>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2> &&t)
        {
            return [=]() { theory(std::get<0>(t),
                                  std::get<1>(t),
                                  std::get<2>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2, typename TArg3>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2, TArg3> &&t)
        {
            return [=]() { theory(std::get<0>(t),
                                  std::get<1>(t),
                                  std::get<2>(t),
                                  std::get<3>(t)); };
        }

        template<typename TFn, typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
        static std::function<void()> TheoryHelper(TFn &&theory, std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4> &&t)
        {
            return [=]() { theory(std::get<0>(t),
                                  std::get<1>(t),
                                  std::get<2>(t),
                                  std::get<3>(t),
                                  std::get<4>(t)); };
        }

        static std::deque<std::string> SplitParams(std::string &&params);

        // !!!VS something else that can be simplified once VS understands variadic macros...
        template<typename TArg0>
        std::string GetLongTheoryName(std::string name, const std::deque<std::string> &params, std::tuple<TArg0> &&t) const
        {
            name += params[0] + ": " + ToString(std::get<0>(std::forward<std::tuple<TArg0>>(t))) + ")";

            return name;
        }

        template<typename TArg0, typename TArg1>
        std::string GetLongTheoryName(std::string name, const std::deque<std::string> &params, std::tuple<TArg0, TArg1> &&t) const
        {
            name += params[0] + ": " + ToString(std::get<0>(std::forward<std::tuple<TArg0, TArg1>>(t))) + ", ";
            name += params[1] + ": " + ToString(std::get<1>(std::forward<std::tuple<TArg0, TArg1>>(t))) + ")";

            return name;
        }

        template<typename TArg0, typename TArg1, typename TArg2>
        std::string GetLongTheoryName(std::string name, const std::deque<std::string> &params, std::tuple<TArg0, TArg1, TArg2> &&t) const
        {
            name += params[0] + ": " + ToString(std::get<0>(std::forward<std::tuple<TArg0, TArg1, TArg2>>(t))) + ", ";
            name += params[1] + ": " + ToString(std::get<1>(std::forward<std::tuple<TArg0, TArg1, TArg2>>(t))) + ", ";
            name += params[2] + ": " + ToString(std::get<2>(std::forward<std::tuple<TArg0, TArg1, TArg2>>(t))) + ")";

            return name;
        }

        template<typename TArg0, typename TArg1, typename TArg2, typename TArg3>
        std::string GetLongTheoryName(std::string name, const std::deque<std::string> &params, std::tuple<TArg0, TArg1, TArg2, TArg3> &&t) const
        {
            name += params[0] + ": " + ToString(std::get<0>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3>>(t))) + ", ";
            name += params[1] + ": " + ToString(std::get<1>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3>>(t))) + ", ";
            name += params[2] + ": " + ToString(std::get<2>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3>>(t))) + ", ";
            name += params[3] + ": " + ToString(std::get<3>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3>>(t))) + ")";

            return name;
        }

        template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
        std::string GetLongTheoryName(std::string name, const std::deque<std::string> &params, std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4> &&t) const
        {
            name += params[0] + ": " + ToString(std::get<0>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4>>(t))) + ", ";
            name += params[1] + ": " + ToString(std::get<1>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4>>(t))) + ", ";
            name += params[2] + ": " + ToString(std::get<2>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4>>(t))) + ", ";
            name += params[3] + ": " + ToString(std::get<3>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4>>(t))) + ", ";
            name += params[4] + ": " + ToString(std::get<4>(std::forward<std::tuple<TArg0, TArg1, TArg2, TArg3, TArg4>>(t))) + ")";

            return name;
        }

    public:
        Register(TestCollection &collection, std::function<void()> &&fn, std::string &&name, const std::string &suite,
            AttributeCollection &&attributes, int milliseconds, std::string &&filename, int line, std::vector<std::shared_ptr<TestEventRecorder>> &&testEventRecorders);

        template<typename TTheory, typename TTheoryData>
        Register(TestCollection &collection, TTheory &&theory_, TTheoryData &&theoryData_, std::string &&name, const std::string &suite, std::string &&params,
            const AttributeCollection &attributes, int milliseconds, std::string &&filename, int line, const std::vector<std::shared_ptr<TestEventRecorder>> &testEventRecorders)
        {
            auto theory = theory_;
            auto theoryData = theoryData_;

            int id = 0;
            for (auto t : theoryData())
            {
                auto theoryName = GetLongTheoryName(name + " [" + ToString(id++) + "] (", SplitParams(std::move(params)), std::forward<decltype(t)>(t));

                collection.mTests.push_back(std::make_shared<xUnitTest>(TheoryHelper(std::forward<TTheory>(theory), std::move(t)), std::move(theoryName), std::move(name), suite,
                    attributes, Time::ToDuration(Time::ToMilliseconds(milliseconds)), std::move(filename), line, testEventRecorders));
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
