#include "xUnit++/IOutput.h"
#include "xUnit++/xUnit++.h"
#include "xUnit++/xUnitTestRunner.h"
#include "Helpers/OutputRecord.h"

SUITE("LineInfo")
{

FACT("LineInfoOverridesDefaultTestLineInfo")
{
    auto file = "filename";
    auto line = 1;
    auto test = [=]()
    {
        Assert.Fail(xUnitpp::LineInfo(file, line));
    };

    xUnitpp::Tests::OutputRecord record;
    xUnitpp::AttributeCollection attributes;
    xUnitpp::TestCollection collection;
    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> localEventRecorders;
    xUnitpp::TestCollection::Register reg(collection, test,
        "LineInfoOverridesDefaultTestLineInfo", "LineInfo", std::forward<decltype(attributes)>(attributes),
        -1, __FILE__, __LINE__, std::forward<decltype(localEventRecorders)>(localEventRecorders));

    xUnitpp::RunTests(record, [](const xUnitpp::TestDetails &) { return true; }, collection.Tests(), xUnitpp::Time::Duration::zero(), 0);

    Assert.Equal(1U, record.events.size());
    Assert.Equal(file, std::get<1>(record.events[0]).LineInfo().file);
    Assert.Equal(line, std::get<1>(record.events[0]).LineInfo().line);
}

}
