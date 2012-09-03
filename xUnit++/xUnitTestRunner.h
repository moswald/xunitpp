#ifndef XUNITTESTRUNNER_H_
#define XUNITTESTRUNNER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "xUnitTime.h"

namespace xUnitpp
{

struct IOutput;
struct TestDetails;
class xUnitTest;

class TestRunner
{
public:
    TestRunner(IOutput &testReporter);
    int RunTests(std::function<bool(const TestDetails &)> filter, const std::vector<xUnitTest> &tests,
                 Time::Duration maxTestRunTime, size_t maxConcurrent);

private:
    class Impl;
    std::shared_ptr<Impl> mImpl;
};

}

#endif
