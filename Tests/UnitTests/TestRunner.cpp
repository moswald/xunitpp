#include <thread>
#include "xUnit++/xUnit++.h"
#include "xUnit++/xUnitTestRunner.h"
#include "xUnit++/xUnitTime.h"
#include "Helpers/OutputRecord.h"

using xUnitpp::Assert;
using xUnitpp::xUnitTest;
using xUnitpp::RunTests;
namespace Tests = xUnitpp::Tests;
namespace Time = xUnitpp::Time;

SUITE(TestRunner)
{

struct TestFactory
{
    TestFactory(std::function<void()> testFn, std::shared_ptr<xUnitpp::Check> check)
        : testFn(testFn)
        , check(check)
        , timeLimit(-1)
        , file("dummy.cpp")
        , line(0)
    {
    }

    TestFactory &Name(const std::string &name)
    {
        this->name = name;
        return *this;
    }

    TestFactory &Suite(const std::string &suite)
    {
        this->suite = suite;
        return *this;
    }

    TestFactory &Duration(Time::Duration timeLimit)
    {
        this->timeLimit = timeLimit;
        return *this;
    }

    TestFactory &Attributes(const xUnitpp::AttributeCollection &attributes)
    {
        this->attributes = attributes;
        return *this;
    }

    TestFactory &TestFile(const std::string &file)
    {
        this->file = file;
        return *this;
    }

    TestFactory &TestLine(int line)
    {
        this->line = line;
        return *this;
    }

    operator std::shared_ptr<xUnitTest>() const
    {
        return std::make_shared<xUnitTest>(testFn, name, suite, attributes, timeLimit, file, line, check);
    }

private:
    std::function<void()> testFn;
    std::shared_ptr<xUnitpp::Check> check;
    std::string name;
    std::string suite;
    Time::Duration timeLimit;
    xUnitpp::AttributeCollection attributes;
    std::string file;
    int line;
};

struct TestRunnerFixture
{
    TestRunnerFixture()
        : testCheck(std::make_shared<xUnitpp::Check>())
    {
    }

    std::shared_ptr<xUnitpp::Check> testCheck;
    std::vector<std::shared_ptr<xUnitTest>> tests;
    Tests::OutputRecord output;
    Time::Duration duration;
};

namespace Filter
{
    bool AllTests(const xUnitpp::TestDetails &) { return true; }
    bool NoTests(const xUnitpp::TestDetails &) { return false; }
}

struct EmptyTest
{
    void operator()() const
    {
    }
};

struct FailingTest
{
    void operator()() const
    {
        Assert.Fail();
    }
};

struct SleepyTest
{
    SleepyTest(int ms = 20)
        : duration(ms)
    {
    }

    void operator()() const
    {
        std::this_thread::sleep_for(duration);
    }

    std::chrono::milliseconds duration;
};

FACT_FIXTURE(TestStartIsReported, TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testCheck).Name("started"));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.orderedTestList.size());
    Assert.Equal("started", output.orderedTestList[0].Name);
}

FACT_FIXTURE(TestFinishIsReported, TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testCheck).Name("finished"));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.finishedTests.size());
    Assert.Equal("finished", std::get<0>(output.finishedTests[0]).Name);
}

FACT_FIXTURE(NoTestsAreFailuresWhenNoTestsRun, TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testCheck).Name("not run"));

    Assert.Equal(0, RunTests(output, &Filter::NoTests, tests, duration, 0));
    Assert.Equal(0U, output.orderedTestList.size());
    Assert.Equal(0U, output.finishedTests.size());
}

FACT_FIXTURE(FailureIsReportedOncePerAssert, TestRunnerFixture)
{
    tests.push_back(TestFactory(FailingTest(), testCheck).Name("failing"));
    tests.push_back(TestFactory(EmptyTest(), testCheck).Name("empty"));
    tests.push_back(TestFactory(FailingTest(), testCheck).Name("failing"));

    Assert.Equal(2, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(2U, output.failures.size());
    
}

FACT_FIXTURE(TestsAbortOnFirstAssert, TestRunnerFixture)
{
    tests.push_back(TestFactory([]() { Assert.Fail() << "first"; Assert.Fail() << "second"; }, testCheck));

    Assert.Equal(1, RunTests(output, &Filter::AllTests, tests, duration, 0), LI);
    Assert.Equal(1U, output.failures.size(), LI);
    Assert.Contains(std::get<1>(output.failures[0]), "first", LI);
    Assert.DoesNotContain(std::get<1>(output.failures[0]), "second", LI);
}

FACT_FIXTURE(FailureIsReportedOncePerCheck, TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testCheck->Fail(); }, testCheck));

    Assert.Equal(1, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(1U, output.failures.size());
}

FACT_FIXTURE(TestsDoNotAbortOnCheck, TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testCheck->Fail() << "first"; testCheck->Fail() << "second"; }, testCheck));

    Assert.Equal(1, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(2U, output.failures.size());
    Assert.Contains(std::get<1>(output.failures[0]), "first");
    Assert.Contains(std::get<1>(output.failures[1]), "second");
}

FACT_FIXTURE(TestCountIsReported, TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summaryCount);
}

FACT_FIXTURE(FailedTestsAreReported, TestRunnerFixture)
{
    tests.push_back(TestFactory(FailingTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summaryFailed);
}

FACT_FIXTURE(FailuresAreReported, TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testCheck->Fail(); testCheck->Fail(); }, testCheck));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(2U, output.failures.size());
}

FACT_FIXTURE(SkippedTestsAreReported, TestRunnerFixture)
{
    xUnitpp::AttributeCollection attributes;
    attributes.insert(std::make_pair("Skip", ""));

    tests.push_back(TestFactory(FailingTest(), testCheck).Attributes(attributes));

    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summarySkipped);
}

UNTIMED_FACT_FIXTURE(SlowTestsPassHighTimeThreshold, TestRunnerFixture)
{
    tests.push_back(TestFactory(SleepyTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(200)), 0);

    Assert.Equal(0U, output.failures.size());
    Assert.Equal(0U, output.summaryFailed);
}

UNTIMED_FACT_FIXTURE(SlowTestsFailLowTimeThreshold, TestRunnerFixture)
{
    SleepyTest sleepyTest;
    tests.push_back(TestFactory(sleepyTest, testCheck));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0);

    Assert.Equal(1U, output.failures.size());
    Assert.Equal(1U, output.summaryFailed);
}

UNTIMED_FACT_FIXTURE(SlowTestFailsBecauseOfTimeLimitReportsReason, TestRunnerFixture)
{
    tests.push_back(TestFactory(SleepyTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0);

    Assert.Equal(1U, output.failures.size());
    Assert.Contains(std::get<1>(output.failures[0]), "Test failed to complete within");
    Assert.Contains(std::get<1>(output.failures[0]), "1 milliseconds.");
}

UNTIMED_FACT_FIXTURE(SlowTestWithTimeExemptionPasses, TestRunnerFixture)
{
    tests.push_back(TestFactory(SleepyTest(), testCheck).Duration(Time::ToDuration(Time::ToMilliseconds(0))));

    Assert.Equal(0, RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0));
}

FACT_FIXTURE(AllTestsAreRunWithNoFilter, TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summaryCount);
}

FACT_FIXTURE(FilteredTestsDoNotRun, TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testCheck));
    RunTests(output, &Filter::NoTests, tests, duration, 0);

    Assert.Equal(0U, output.summaryCount);
}

}
