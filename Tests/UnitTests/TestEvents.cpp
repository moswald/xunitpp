#include <vector>
#include <memory>
#include "xUnit++/xUnit++.h"
#include "xUnit++/EventLevel.h"
#include "xUnit++/TestEvent.h"
#include "xUnit++/TestEventRecorder.h"
#include "xUnit++/TestCollection.h"
#include "xUnit++/xUnitTestRunner.h"
#include "Helpers/OutputRecord.h"

SUITE("TestEvents")
{

struct Fixture
{
    Fixture()
    {
        localEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
        localEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
        localEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());

        localCheck = std::make_shared<xUnitpp::Check>(*localEventRecorders[0]);
        localWarn = std::make_shared<xUnitpp::Warn>(*localEventRecorders[1]);
        localLog = std::make_shared<xUnitpp::Log>(*localEventRecorders[2]);
    }

    void Run()
    {
        xUnitpp::RunTests(outputRecord, [](const xUnitpp::ITestDetails &) { return true; }, collection.Tests(), xUnitpp::Time::Duration::zero(), 0);
    }

    const xUnitpp::Check &LocalCheck() const
    {
        return *localCheck;
    }

    const xUnitpp::Warn &LocalWarn() const
    {
        return *localWarn;
    }

    const xUnitpp::Log &LocalLog() const
    {
        return *localLog;
    }

    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> localEventRecorders;
    xUnitpp::TestCollection collection;
    xUnitpp::Tests::OutputRecord outputRecord;

private:
    std::shared_ptr<xUnitpp::Check> localCheck;
    std::shared_ptr<xUnitpp::Warn> localWarn;
    std::shared_ptr<xUnitpp::Log> localLog;
};

FACT_FIXTURE("All TestEvents should be ordered", Fixture)
{
    auto factWithEvents = [&]()
    {
        LocalWarn().Fail();
        LocalCheck().Fail();
        LocalWarn().False(true);

        LocalLog().Debug << "debug message";
        LocalLog().Info << "info message";
        LocalLog().Warn << "warning message";
    };

    xUnitpp::TestCollection::Register reg(collection, factWithEvents, "Name", "Suite", xUnitpp::AttributeCollection(), -1, "file", 0, std::forward<decltype(localEventRecorders)>(localEventRecorders));
    (void)reg;

    Run();

    Assert.Equal(xUnitpp::EventLevel::Warning, std::get<1>(outputRecord.events[0]).GetLevel());
    Assert.Equal(xUnitpp::EventLevel::Check, std::get<1>(outputRecord.events[1]).GetLevel());
    Assert.Equal(xUnitpp::EventLevel::Warning, std::get<1>(outputRecord.events[2]).GetLevel());

    Assert.Contains(to_string(std::get<1>(outputRecord.events[0])), "Fail");
    Assert.Contains(to_string(std::get<1>(outputRecord.events[2])), "False");

    xUnitpp::EventLevel expectedLevels[] = {
        xUnitpp::EventLevel::Warning, xUnitpp::EventLevel::Check, xUnitpp::EventLevel::Warning,
        xUnitpp::EventLevel::Debug, xUnitpp::EventLevel::Info, xUnitpp::EventLevel::Warning
    };

    Assert.Equal(std::begin(expectedLevels), std::end(expectedLevels), outputRecord.events.begin(), outputRecord.events.end(),
        [](xUnitpp::EventLevel lvl, const std::tuple<xUnitpp::TestDetails, xUnitpp::TestEvent> &result)
        {
            return lvl == std::get<1>(result).GetLevel();
        });
}

FACT_FIXTURE("TestEventSources should be usable within Theories", Fixture)
{
    auto theoryWithChecks = [&](int x) { LocalCheck().Fail() << "id: " << x; };

    std::vector<std::tuple<int>> theoryData;
    for (int i = 0; i != 10; ++i)
    {
        theoryData.push_back(std::make_tuple(i));
    }

    xUnitpp::TestCollection::Register reg(collection, theoryWithChecks, [&]() { return theoryData; },
        "Name", "Suite", "(int x)", xUnitpp::AttributeCollection(), -1, "file", 0, localEventRecorders);
    (void)reg;

    Run();

    for (const auto &test : collection.Tests())
    {
        Check.Equal(1U, test->TestEvents().size()) << "current: " << test->TestDetails().Id;
    }
}

}
