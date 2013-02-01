#include "TestFactory.h"
#include "xUnit++/TestEventRecorder.h"
#include "xUnit++/xUnitTest.h"

namespace xUnitpp { namespace Tests {

TestFactory::TestFactory(std::function<void()> testFn)
    : testFn(testFn)
    , testEventRecorders()
    , timeLimit(-1)
    , file("dummy.cpp")
    , line(0)
{
}

TestFactory::TestFactory(std::function<void()> testFn, std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> testEventRecorders)
    : testFn(testFn)
    , testEventRecorders(testEventRecorders)
    , timeLimit(-1)
    , file("dummy.cpp")
    , line(0)
{
}

TestFactory &TestFactory::Name(const std::string &name)
{
    this->name = name;
    return *this;
}

TestFactory &TestFactory::Suite(const std::string &suite)
{
    this->suite = suite;
    return *this;
}

TestFactory &TestFactory::Duration(Time::Duration timeLimit)
{
    this->timeLimit = timeLimit;
    return *this;
}

TestFactory &TestFactory::Attributes(const xUnitpp::AttributeCollection &attributes)
{
    this->attributes = attributes;
    return *this;
}

TestFactory &TestFactory::TestFile(const std::string &file)
{
    this->file = file;
    return *this;
}

TestFactory &TestFactory::TestLine(int line)
{
    this->line = line;
    return *this;
}

TestFactory::operator std::shared_ptr<xUnitTest>()
{
    return std::make_shared<xUnitTest>(std::move(testFn), std::string(name), 0, "", suite, std::move(attributes), timeLimit, std::move(file), line, testEventRecorders);
}

}}
