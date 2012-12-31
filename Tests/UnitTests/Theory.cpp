#include <algorithm>
#include <array>
#include <mutex>
#include <string>
#include <tuple>
#include <vector>
#include "xUnit++/IOutput.h"
#include "xUnit++/xUnitTestRunner.h"
#include "xUnit++/xUnitTime.h"
#include "xUnit++/xUnit++.h"
#include "Helpers/OutputRecord.h"

SUITE("Theory")
{

void TheoryUnderTest(int x)
{
    xUnitpp::Assert.True(x == 0 || x == 1);
}

struct TheoryFixture
{
public:
    TheoryFixture()
        : fakeFileName("FakeFile.cpp")
    {
    }

    template<typename TTheoryData>
    void Register(std::string &&name, std::string &&params, TTheoryData &&theoryData)
    {
        xUnitpp::TestCollection::Register reg(
            collection,
            &TheoryUnderTest,
            std::forward<TTheoryData>(theoryData),
            std::move(name),
            "Theory",
            std::move(params),
            attributes,
            -1,
            GetFakeFileName(), __LINE__,
            localEventRecorders);
        (void)reg;
    }

    void Run()
    {
        RunTests(record, [](const xUnitpp::ITestDetails &) { return true; },
            collection.Tests(), xUnitpp::Time::Duration::zero(), 0);
    }

    template<typename TTheoryData>
    void RegisterAndRun(std::string &&name, std::string &&params, TTheoryData &&theoryData)
    {
        Register(std::move(name), std::move(params), std::forward<TTheoryData>(theoryData));

        Run();
    }

    std::string GetFakeFileName() const
    {
        return fakeFileName;
    }

    std::string fakeFileName;

    xUnitpp::Tests::OutputRecord record;
    xUnitpp::AttributeCollection attributes;
    xUnitpp::TestCollection collection;
    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> localEventRecorders;
};

std::vector<std::tuple<int>> RawFunctionProvider()
{
    std::tuple<int> tuples[] =
    {
        std::make_tuple(0),
        std::make_tuple(1),
        std::make_tuple(2),
        std::make_tuple(3),
        std::make_tuple(0)
    };

    return std::vector<std::tuple<int>>(std::begin(tuples), std::end(tuples));
}

FACT_FIXTURE("TheoriesAcceptRawFunctions", TheoryFixture)
{
    RegisterAndRun("TheoriesAcceptRawFunctions", "(int x)", RawFunctionProvider);
}

FACT_FIXTURE("TheoriesAcceptStdFunction", TheoryFixture)
{
    std::function<std::vector<std::tuple<int>>()> provider = &RawFunctionProvider;

    RegisterAndRun("TheoriesAcceptStdFunction", "(int x)", provider);
}

FACT_FIXTURE("TheoriesAcceptFunctors", TheoryFixture)
{
    struct
    {
        std::vector<std::tuple<int>> operator()() const
        {
            return RawFunctionProvider();
        }
    } functor;

    RegisterAndRun("TheoriesAcceptFunctors", "(int x)", functor);
}

FACT_FIXTURE("TheoriesGetAllDataPassedToThem", TheoryFixture)
{
    std::mutex lock;
    std::vector<int> dataProvided;

    auto doTheory = [&](int x) { std::lock_guard<std::mutex> guard(lock); dataProvided.push_back(x); };
    xUnitpp::TestCollection::Register reg(collection, doTheory, RawFunctionProvider,
        "TheoriesGetAllDataPassedToThem", "Theory", "(int x)", attributes, -1, GetFakeFileName(), __LINE__, localEventRecorders);
    (void)reg;

    Run();

    Assert.Equal(2, std::count(dataProvided.begin(), dataProvided.end(), 0), LI);
    Assert.Equal(1, std::count(dataProvided.begin(), dataProvided.end(), 1), LI);
    Assert.Equal(1, std::count(dataProvided.begin(), dataProvided.end(), 2), LI);
    Assert.Equal(1, std::count(dataProvided.begin(), dataProvided.end(), 3), LI);
}

FACT_FIXTURE("TheoriesCanBeSkipped", TheoryFixture)
{
    attributes.insert(std::make_pair("Skip", "Testing skip."));

    auto doTheory = [](int) { Assert.Fail() << "Should not be run."; };

    xUnitpp::TestCollection::Register reg(collection, doTheory, RawFunctionProvider,
        "TheoriesGetAllDataPassedToThem", "Theory", "(int x)", attributes, -1, GetFakeFileName(), __LINE__, localEventRecorders);
    (void)reg;

    Run();
}

std::vector<std::tuple<std::string, std::vector<std::tuple<int, std::string>>>> ComplexProvider()
{
    std::vector<std::tuple<std::string, std::vector<std::tuple<int, std::string>>>> result;

    std::vector<std::tuple<int, std::string>> internal_vector;
    internal_vector.push_back(std::make_tuple(0, std::string("xyz")));

    result.push_back(std::make_tuple(std::string("abcd"), internal_vector));

    return result;
}

DATA_THEORY("TheoriesCanAcceptComplexObjects", (const std::string &, const std::vector<std::tuple<int, std::string>> &), ComplexProvider)
{
    // just existing is good enough
}

DATA_THEORY("TheoriesCanAcceptLambdas", (int, int),
    ([]() -> std::vector<std::tuple<int, int>>
    {
        std::vector<std::tuple<int, int>> data;
        data.push_back(std::make_tuple(0, 0));
        return data;
    })
)
{
    // just existing is good enough
}

THEORY("TestingSuccessfulInlineTheory", (int x, const std::string &str),
    std::make_tuple(0, ""),
    std::make_tuple(0, ""),
    std::make_tuple(0, ""),
    std::make_tuple(0, ""))
{
    Assert.Equal(0, x);
    Assert.Empty(str);
}

THEORY("TestingFailingInlineTheory", (int, const std::string &str),
    std::make_tuple(1, "a"),
    std::make_tuple(1, "b"),
    std::make_tuple(1, "c"),
    std::make_tuple(1, "d"))
{
    Assert.Throws<xUnitpp::xUnitAssert>([&]() { Assert.Empty(str); });
}

DATA_THEORY("TestingLambdasAsData", (std::function<void()> fn, int),
([]() -> std::vector<std::tuple<std::function<void()>, int>>
{
    std::vector<std::tuple<std::function<void()>, int>> data;
    data.emplace_back([]() { }, 0);
    return data;
})
)
{
    Assert.DoesNotThrow(fn);
}

struct ArrayProvider
{
    const std::array<std::tuple<int>, 1> &operator ()() const
    {
        static std::array<std::tuple<int>, 1> tuples;
        tuples[0] = std::make_tuple(0);
        return tuples;
    }
};

struct CustomProvider
{
    CustomProvider()
    {
        tuples[0] = std::make_tuple(0);
    }

    const CustomProvider &operator ()() const
    {
        return *this;
    }

    friend const std::tuple<int> *begin(const CustomProvider &cp)
    {
        return &cp.tuples[0];
    }

    friend const std::tuple<int> *end(const CustomProvider &cp)
    {
        return &cp.tuples[1];
    }

    std::tuple<int> tuples[1];
};

FACT_FIXTURE("DATA_THEORY accepts anything that has begin/end", TheoryFixture)
{
    Register("Array", "(int)", ArrayProvider());
    Register("Custom", "(int)", CustomProvider());

    Run();
}

FACT_FIXTURE("Every theory instance should have a name", TheoryFixture)
{
    auto dataProvider =
        []()
        {
            std::vector<std::tuple<int>> dataProvider;
            dataProvider.emplace_back(10);
            dataProvider.emplace_back(20);
            return dataProvider;
        };
    
    RegisterAndRun("TheoryName", "(int x)", std::move(dataProvider));

    Assert.Equal(2U, record.events.size());

    for (auto &&t: record.events)
    {
        auto test = std::get<0>(t);

        Assert.True(test.Name.substr(0, 10) == "TheoryName");
    }
}

FACT_FIXTURE("Every theory instance should have a short name", TheoryFixture)
{
    auto dataProvider =
        []()
        {
            std::vector<std::tuple<int>> dataProvider;
            dataProvider.emplace_back(10);
            dataProvider.emplace_back(20);
            return dataProvider;
        };
    
    RegisterAndRun("TheoryName", "(int x)", std::move(dataProvider));

    Assert.Equal(2U, record.events.size());

    for (auto &&t: record.events)
    {
        auto test = std::get<0>(t);

        Assert.Equal("TheoryName", test.Name);
    }
}

FACT_FIXTURE("Every theory instance should know the file it is in", TheoryFixture)
{
    auto dataProvider =
        []()
        {
            std::vector<std::tuple<int>> dataProvider;
            dataProvider.emplace_back(10);
            dataProvider.emplace_back(20);
            return dataProvider;
        };
    
    RegisterAndRun("TheoryName", "(int x)", std::move(dataProvider));

    Assert.Equal(2U, record.events.size());

    for (auto &&t: record.events)
    {
        auto test = std::get<0>(t);

        Assert.Equal(fakeFileName, test.LineInfo.file);
    }
}

ATTRIBUTES(("Cats", "Meow"))
{
DATA_THEORY("TheoriesCanHaveAttributes", (int), RawFunctionProvider)
{
    for (const auto &test : xUnitpp::TestCollection::Instance().Tests())
    {
        if (test->TestDetails().Name == "TheoriesCanHaveAttributes")
        {
            auto it = std::find_if(test->TestDetails().Attributes.begin(), test->TestDetails().Attributes.end(), [](const std::pair<std::string, std::string> &item) { return item.first == "Cats"; });
            Assert.True(it != test->TestDetails().Attributes.end());
            Assert.True(it->second == "Meow");
            return;
        }
    }

    Assert.Fail() << "Could not find self in test list.";
}
}

}
