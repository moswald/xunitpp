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

xUnitTest::xUnitTest(const xUnitTest &other)
    : mTest(other.mTest)
    , mTestDetails(other.mTestDetails)
    , mCheck(other.mCheck)
{
}

xUnitTest::xUnitTest(xUnitTest &&other)
{
    swap(*this, other);
}

xUnitTest &xUnitTest::operator =(xUnitTest other)
{
    swap(*this, other);
    return *this;
}

void swap(xUnitTest &a, xUnitTest &b)
{
    using std::swap;

    swap(a.mTest, b.mTest);
    swap(a.mTestDetails, b.mTestDetails);
    swap(a.mCheck, b.mCheck);
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
