#include "xUnitTest.h"
#include "xUnitCheck.h"

namespace xUnitpp
{

xUnitTest::xUnitTest(std::function<void()> test, const std::string &name, const std::string &suite,
                     const AttributeCollection &attributes, Time::Duration timeLimit,
                     const std::string &filename, int line, std::shared_ptr<Check> check)
    : mTest(test)
    , mTestDetails(name, suite, attributes, timeLimit, filename, line)
    , mCheck(check)
{
}

const TestDetails &xUnitTest::TestDetails() const
{
    return mTestDetails;
}

void xUnitTest::Run()
{
    return mTest();
}

const std::vector<xUnitAssert> &xUnitTest::NonFatalFailures() const
{
    return mCheck->Failures();
}

}
