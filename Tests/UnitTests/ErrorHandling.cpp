#include "xUnit++/xUnit++.h"
#include "xUnit++/xUnitTestRunner.h"
#include "Helpers/OutputRecord.h"

SUITE("Error Handling")
{

#if defined(_MSC_VER)
FACT("A test that divides by zero should not halt the test runner")
{
    auto test = []()
    {
        volatile auto z = 0;
        volatile auto y = 1;

        volatile auto x = y / z;
        (void)x;
    };

    xUnitpp::TestCollection collection;
    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> localEventRecorders;
    localEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
    localEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
    localEventRecorders.push_back(std::make_shared<xUnitpp::TestEventRecorder>());
    xUnitpp::Tests::OutputRecord outputRecord;
    
    xUnitpp::TestCollection::Register reg(collection, test, "Name", "Suite", xUnitpp::AttributeCollection(), -1, "file", 0, std::forward<decltype(localEventRecorders)>(localEventRecorders));
    (void)reg;

    xUnitpp::RunTests(outputRecord, [](const xUnitpp::ITestDetails &) { return true; }, collection.Tests(), xUnitpp::Time::Duration::zero(), 0);
}
#endif

}
