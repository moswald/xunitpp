#ifndef TESTFACTORY_H_
#define TESTFACTORY_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "xUnit++/Attributes.h"
#include "xUnit++/xUnitTime.h"

namespace xUnitpp
{
    class TestEventRecorder;
    class xUnitTest;
}

namespace xUnitpp { namespace Tests {

struct TestFactory
{
    TestFactory(std::function<void()> testFn);
    TestFactory(std::function<void()> testFn, std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> testEventRecorders);

    TestFactory &Name(const std::string &name);
    TestFactory &Suite(const std::string &suite);
    TestFactory &Duration(Time::Duration timeLimit);
    TestFactory &Attributes(const xUnitpp::AttributeCollection &attributes);
    TestFactory &TestFile(const std::string &file);
    TestFactory &TestLine(int line);
    operator std::shared_ptr<xUnitTest>();

private:
    std::function<void()> testFn;
    std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> testEventRecorders;
    std::string name;
    std::string suite;
    Time::Duration timeLimit;
    xUnitpp::AttributeCollection attributes;
    std::string file;
    int line;
};

}}

#endif
