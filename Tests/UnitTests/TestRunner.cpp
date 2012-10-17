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

SUITE("TestRunner")
{

struct TestFactory
{
    TestFactory(std::function<void()> testFn, std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> testEventRecorders)
        : testFn(testFn)
        , testEventRecorders(testEventRecorders)
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

    operator std::shared_ptr<xUnitTest>()
    {
        return std::make_shared<xUnitTest>(std::move(testFn), std::move(name), std::move(name), suite, attributes, timeLimit, std::move(file), line, testEventRecorders);
    }

private:
    std::function<void()> testFn;
    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> testEventRecorders;
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
    {
        testEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
        testEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
        testCheck = std::make_shared<xUnitpp::Check>(*testEventRecorders[0]);
        testWarn = std::make_shared<xUnitpp::Warn>(*testEventRecorders[1]);
    }

    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> testEventRecorders;
    std::shared_ptr<xUnitpp::Check> testCheck;
    std::shared_ptr<xUnitpp::Warn> testWarn;
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

FACT_FIXTURE("TestStartIsReported", TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders).Name("started"));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.orderedTestList.size());
    Assert.Equal("started", output.orderedTestList[0].Name);
}

FACT_FIXTURE("TestFinishIsReported", TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders).Name("finished"));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.finishedTests.size());
    Assert.Equal("finished", std::get<0>(output.finishedTests[0]).Name);
}

FACT_FIXTURE("NoTestsAreFailuresWhenNoTestsRun", TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders).Name("not run"));

    Assert.Equal(0, RunTests(output, &Filter::NoTests, tests, duration, 0));
    Assert.Equal(0U, output.orderedTestList.size());
    Assert.Equal(0U, output.finishedTests.size());
}

FACT_FIXTURE("FailureIsReportedOncePerAssert", TestRunnerFixture)
{
    tests.push_back(TestFactory(FailingTest(), testEventRecorders).Name("failing"));
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders).Name("empty"));
    tests.push_back(TestFactory(FailingTest(), testEventRecorders).Name("failing"));

    Assert.Equal(2, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(2U, output.events.size());
    
}

FACT_FIXTURE("TestsAbortOnFirstAssert", TestRunnerFixture)
{
    tests.push_back(TestFactory([]() { Assert.Fail() << "first"; Assert.Fail() << "second"; }, testEventRecorders));

    Assert.Equal(1, RunTests(output, &Filter::AllTests, tests, duration, 0), LI);
    Assert.Equal(1U, output.events.size(), LI);
    Assert.Contains(to_string(std::get<1>(output.events[0])), "first", LI);
    Assert.DoesNotContain(to_string(std::get<1>(output.events[0])), "second", LI);
}

FACT_FIXTURE("FailureIsReportedOncePerCheck", TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testCheck->Fail(); }, testEventRecorders));

    Assert.Equal(1, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(1U, output.events.size());
}

FACT_FIXTURE("TestsDoNotAbortOnCheck", TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testCheck->Fail() << "first"; testCheck->Fail() << "second"; }, testEventRecorders));

    Assert.Equal(1, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(2U, output.events.size());
    Assert.Contains(to_string(std::get<1>(output.events[0])), "first");
    Assert.Contains(to_string(std::get<1>(output.events[1])), "second");
}

FACT_FIXTURE("FailuresAreReported", TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testCheck->Fail(); testCheck->Fail(); }, testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(2U, output.events.size());
}

FACT_FIXTURE("TestCountIsReported", TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summaryCount);
}

FACT_FIXTURE("FailedTestsAreReported", TestRunnerFixture)
{
    tests.push_back(TestFactory(FailingTest(), testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summaryFailed);
}

FACT_FIXTURE("SkippedTestsAreReported", TestRunnerFixture)
{
    xUnitpp::AttributeCollection attributes;
    attributes.insert(std::make_pair("Skip", ""));

    tests.push_back(TestFactory(FailingTest(), testEventRecorders).Attributes(attributes));

    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summarySkipped);
}

UNTIMED_FACT_FIXTURE("SlowTestsPassHighTimeThreshold", TestRunnerFixture)
{
    tests.push_back(TestFactory(SleepyTest(), testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(200)), 0);

    Assert.Equal(0U, output.events.size());
    Assert.Equal(0U, output.summaryFailed);
}

UNTIMED_FACT_FIXTURE("SlowTestsFailLowTimeThreshold", TestRunnerFixture)
{
    SleepyTest sleepyTest;
    tests.push_back(TestFactory(sleepyTest, testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0);

    Assert.Equal(1U, output.events.size());
    Assert.Equal(1U, output.summaryFailed);
}

UNTIMED_FACT_FIXTURE("SlowTestFailsBecauseOfTimeLimitReportsReason", TestRunnerFixture)
{
    tests.push_back(TestFactory(SleepyTest(), testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0);

    Assert.Equal(1U, output.events.size());
    Assert.Contains(to_string(std::get<1>(output.events[0])), "Test failed to complete within");
    Assert.Contains(to_string(std::get<1>(output.events[0])), "1 milliseconds.");
}

UNTIMED_FACT_FIXTURE("SlowTestWithTimeExemptionPasses", TestRunnerFixture)
{
    tests.push_back(TestFactory(SleepyTest(), testEventRecorders).Duration(Time::ToDuration(Time::ToMilliseconds(0))));

    Assert.Equal(0, RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0));
}

FACT_FIXTURE("AllTestsAreRunWithNoFilter", TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summaryCount);
}

FACT_FIXTURE("FilteredTestsDoNotRun", TestRunnerFixture)
{
    tests.push_back(TestFactory(EmptyTest(), testEventRecorders));
    RunTests(output, &Filter::NoTests, tests, duration, 0);

    Assert.Equal(0U, output.summaryCount);
}

FACT_FIXTURE("Warnings are not failures", TestRunnerFixture)
{
    tests.push_back(TestFactory([=]() { testWarn->Fail(); }, testEventRecorders));

    Assert.Equal(0, RunTests(output, &Filter::AllTests, tests, duration, 0));
    Assert.Equal(1U, output.events.size());
    Assert.Equal(0U, output.summaryFailed);
}

}
