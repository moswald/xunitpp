#include "xUnitTest.h"

namespace xUnitpp
{

xUnitTest::xUnitTest(std::function<void()> test, const std::string &name, const std::string &suite,
                     const AttributeCollection &attributes, Time::Duration timeLimit,
                     const std::string &filename, int line)
    : mTest(test)
    , mTestDetails(name, suite, attributes, timeLimit, filename, line)
{
}

xUnitTest::xUnitTest(const xUnitTest &other)
    : mTest(other.mTest)
    , mTestDetails(other.mTestDetails)
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
}

const TestDetails &xUnitTest::TestDetails() const
{
    return mTestDetails;
}

void xUnitTest::Run()
{
    return mTest();
}

}
