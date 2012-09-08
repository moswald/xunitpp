#ifndef XUNITTEST_H_
#define XUNITTEST_H_

#include <functional>
#include <string>
#include "TestDetails.h"
#include "xUnitCheck.h"

namespace xUnitpp
{

class xUnitTest
{
public:
    xUnitTest(std::function<void()> test, const std::string &name, const std::string &suite,
        const AttributeCollection &attributes, Time::Duration timeLimit,
        const std::string &filename, int line, const Check &check);
    xUnitTest(const xUnitTest &other);
    xUnitTest(xUnitTest &&other);
    xUnitTest &operator =(xUnitTest other);
    friend void swap(xUnitTest &a, xUnitTest &b);

    const TestDetails &TestDetails() const;

    void Run();

    const std::vector<xUnitAssert> &NonFatalFailures() const;

private:
    std::function<void()> mTest;
    xUnitpp::TestDetails mTestDetails;
    std::reference_wrapper<const Check> mCheck;
};

}

#endif
