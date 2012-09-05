#ifndef XUNITTEST_H_
#define XUNITTEST_H_

#include <functional>
#include <string>
#include "TestDetails.h"

namespace xUnitpp
{

class xUnitTest
{
public:
    xUnitTest(std::function<void()> test, const std::string &name, const std::string &suite,
        const AttributeCollection &attributes, Time::Duration timeLimit,
        const std::string &filename, int line);
    xUnitTest(const xUnitTest &other);
    xUnitTest(xUnitTest &&other);
    xUnitTest &operator =(xUnitTest other);
    friend void swap(xUnitTest &a, xUnitTest &b);

    const TestDetails &TestDetails() const;

    void Run();

private:
    std::function<void()> mTest;
    xUnitpp::TestDetails mTestDetails;
};

}

#endif
