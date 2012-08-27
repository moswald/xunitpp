#include <algorithm>
#include "Fact.h"
#include "IOutput.h"
#include "xUnitTestRunner.h"
#include "xUnit++.h"

using xUnitpp::Assert;

SUITE(Theory)
{

void TheoryUnderTest(int x)
{
    Assert.True(x == 0 || x == 1);
}

struct TheoryFixture
{
private:
    struct EmptyReporter : xUnitpp::IOutput
    {
        virtual void ReportStart(const xUnitpp::TestDetails &, int) override
        {
        }

        virtual void ReportFailure(const xUnitpp::TestDetails &, int, const std::string &) override
        {
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &, const std::string &) override
        {
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &, int, std::chrono::milliseconds) override
        {
        }

        virtual void ReportAllTestsComplete(size_t, size_t, size_t, std::chrono::milliseconds) override 
        {
        }
    };

public:
    TheoryFixture()
        : localRunner(std::make_shared<EmptyReporter>())
    {
    }

    template<typename TTheoryData>
    void Register(const std::string &name, TTheoryData &&theoryData)
    {
        xUnitpp::TestCollection::Register reg(collection, &TheoryUnderTest, theoryData, name, "Theory", attributes, -1, __FILE__, __LINE__);
    }

    void Run()
    {
        localRunner.RunTests([](const xUnitpp::TestDetails &) { return true; },
            collection.Facts(), collection.Theories(), std::chrono::milliseconds::zero(), 0);
    }

    template<typename TTheoryData>
    void RegisterAndRun(const std::string &name, TTheoryData &&theoryData)
    {
        Register(name, theoryData);

        Run();
    }

    xUnitpp::AttributeCollection attributes;
    xUnitpp::TestCollection collection;
    xUnitpp::TestRunner localRunner;
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

FACT_FIXTURE(TheoriesAcceptRawFunctions, TheoryFixture)
{
    RegisterAndRun("TheoriesAcceptRawFunctions", RawFunctionProvider);
}

FACT_FIXTURE(TheoriesAcceptStdFunction, TheoryFixture)
{
    std::function<std::vector<std::tuple<int>>()> provider = RawFunctionProvider;

    RegisterAndRun("TheoriesAcceptStdFunction", provider);
}

FACT_FIXTURE(TheoriesAcceptFunctors, TheoryFixture)
{
    struct
    {
        std::vector<std::tuple<int>> operator()() const
        {
            return RawFunctionProvider();
        }
    } functor;

    RegisterAndRun("TheoriesAcceptFunctors", functor);
}

FACT_FIXTURE(TheoriesGetAllDataPassedToThem, TheoryFixture)
{
    struct localTheory
    {
        void operator()(int x)
        {
            dataProvided.push_back(x);
        }

        std::vector<int> dataProvided;
    } theory;

    auto doTheory = [&](int x) { theory(x); };
    xUnitpp::TestCollection::Register reg(collection, doTheory, RawFunctionProvider, "TheoriesGetAllDataPassedToThem", "Theory", attributes, -1, __FILE__, __LINE__);

    Run();

    Assert.Equal(2, std::count(theory.dataProvided.begin(), theory.dataProvided.end(), 0));
    Assert.Equal(1, std::count(theory.dataProvided.begin(), theory.dataProvided.end(), 1));
    Assert.Equal(1, std::count(theory.dataProvided.begin(), theory.dataProvided.end(), 2));
    Assert.Equal(1, std::count(theory.dataProvided.begin(), theory.dataProvided.end(), 3));
}

FACT_FIXTURE(TheoriesCanBeSkipped, TheoryFixture)
{
    attributes.insert(std::make_pair("Skip", "Testing skip."));

    const struct localTheory
    {
        localTheory()
        {
            Assert.Fail("Should not be instantiated.");
        }

        void operator()(int) const
        {
            Assert.Fail("Should not be run.");
        }
    };

    auto doTheory = [](int x) { localTheory()(x); };

    xUnitpp::TestCollection::Register reg(collection, doTheory, RawFunctionProvider, "TheoriesGetAllDataPassedToThem", "Theory", attributes, -1, __FILE__, __LINE__);

    Run();
}

ATTRIBUTES(TheoriesCanHaveAttributes, ("Cats", "Meow"))
THEORY(TheoriesCanHaveAttributes, (int), RawFunctionProvider)
{
    for (const auto &theory : xUnitpp::TestCollection::Instance().Theories())
    {
        if (theory.TestDetails().Name == "TheoriesCanHaveAttributes")
        {
            auto it = theory.TestDetails().Attributes.find("Cats");
            Assert.True(it != theory.TestDetails().Attributes.end());
            Assert.True(it->second == "Meow");
            return;
        }
    }

    Assert.Fail("Could not find self in test list.");
}

}
