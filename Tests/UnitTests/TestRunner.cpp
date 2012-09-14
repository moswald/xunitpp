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

    operator xUnitTest() const
    {
        return xUnitTest(testFn, name, suite, attributes, timeLimit, file, line, check);
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
    std::vector<xUnitTest> tests;
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

FACT_FIXTURE(TestFinishIncludesCorrectTime, TestRunnerFixture)
{
    auto test = SleepyTest();

    tests.push_back(TestFactory(test, testCheck));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    auto min = Time::ToDuration(test.duration) - Time::ToDuration(Time::ToMilliseconds(5));
    auto max = Time::ToDuration(test.duration) + Time::ToDuration(Time::ToMilliseconds(5));

    Assert.InRange(std::get<1>(output.finishedTests[0]).count(), min.count(), max.count());
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
    tests.push_back(TestFactory(FailingTest(), testCheck));
    tests.push_back(TestFactory([=]() { testCheck->Fail(); testCheck->Fail(); }, testCheck));
    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(3U, output.failures.size());
}

FACT_FIXTURE(SkippedTestsAreReported, TestRunnerFixture)
{
    xUnitpp::AttributeCollection attributes;
    attributes.insert(std::make_pair("Skip", ""));

    tests.push_back(TestFactory(FailingTest(), testCheck).Attributes(attributes));

    RunTests(output, &Filter::AllTests, tests, duration, 0);

    Assert.Equal(1U, output.summarySkipped);
}

TIMED_FACT_FIXTURE(SlowTestsPassHighTimeThreshold, TestRunnerFixture, 0)
{
    tests.push_back(TestFactory(SleepyTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(200)), 0);

    Assert.Equal(0U, output.failures.size());
    Assert.Equal(0U, output.summaryFailed);
}

TIMED_FACT_FIXTURE(SlowTestsFailLowTimeThreshold, TestRunnerFixture, 0)
{
    SleepyTest sleepyTest;
    tests.push_back(TestFactory(sleepyTest, testCheck));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0);

    Assert.Equal(1U, output.failures.size());
    Assert.Equal(1U, output.summaryFailed);
}

TIMED_FACT_FIXTURE(SlowTestFailsBecauseOfTimeLimitReportsReason, TestRunnerFixture, 0)
{
    tests.push_back(TestFactory(SleepyTest(), testCheck));
    RunTests(output, &Filter::AllTests, tests, Time::ToDuration(Time::ToMilliseconds(1)), 0);

    Assert.Equal(1U, output.failures.size());
    Assert.Contains(std::get<1>(output.failures[0]), "Test failed to complete within");
    Assert.Contains(std::get<1>(output.failures[0]), "1 milliseconds.");
}

TIMED_FACT_FIXTURE(SlowTestWithTimeExemptionPasses, TestRunnerFixture, 0)
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

ATTRIBUTES(TestOrderIsRandomized, ("Skip", "Threading issues preventing me from tracking down why this *doesn't* fail right now."))
TIMED_FACT_FIXTURE(TestOrderIsRandomized, TestRunnerFixture, 0)
{
    // the best I can do here is have 10 tests run, and hope that I don't run into the same 10
    // values back-to-back
    int originalOrder[10];
    for (int i = 0; i != 10; ++i)
    {
        originalOrder[i] = i;
        tests.push_back(TestFactory(EmptyTest(), testCheck).TestLine(i));
    }

    RunTests(output, &Filter::AllTests, tests, duration, 1);

    Assert.NotEqual(std::begin(originalOrder), std::end(originalOrder), output.orderedTestList.begin(), output.orderedTestList.end(),
        [](int a, const xUnitpp::TestDetails &b)
        {
            return a == b.Line;
        });
}

}
