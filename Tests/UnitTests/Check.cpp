#include "xUnit++/xUnit++.h"
#include "xUnit++/ITestEventSource.h"
#include "xUnit++/TestCollection.h"
#include "xUnit++/xUnitTestRunner.h"
#include "Helpers/OutputRecord.h"

SUITE("Check")
{

FACT("Check should be usable within Theories")
{
    std::vector<std::shared_ptr<xUnitpp::ITestEventSource>> localEventSources;
    localEventSources.push_back(std::make_shared<xUnitpp::Check>());
    xUnitpp::Check &localCheck = *static_cast<xUnitpp::Check *>(localEventSources[0].get());

    auto theoryWithChecks = [&](int x) { localCheck.Fail() << "id: " << x; };

    std::vector<std::tuple<int>> theoryData;
    for (int i = 0; i != 10; ++i)
    {
        theoryData.push_back(std::make_tuple(i));
    }

    xUnitpp::TestCollection collection;
    xUnitpp::TestCollection::Register reg(collection, theoryWithChecks, [&]() { return theoryData; },
        "Name", "Theory", xUnitpp::AttributeCollection(), -1, "file", 0, localEventSources);

    xUnitpp::Tests::OutputRecord record;

    xUnitpp::RunTests(record, [](const xUnitpp::TestDetails &) { return true; }, collection.Tests(), xUnitpp::Time::Duration::zero(), 0);

    for (const auto &test : collection.Tests())
    {
        Assert.Equal(1U, test->TestEvents().size());
    }
}

}
