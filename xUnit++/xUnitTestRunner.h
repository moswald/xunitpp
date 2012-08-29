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

class Fact;
struct IOutput;
struct TestDetails;
class Theory;

class TestRunner
{
public:
    TestRunner(std::shared_ptr<IOutput> testReporter);
    int RunTests(std::function<bool(const TestDetails &)> filter, const std::vector<Fact> &facts, const std::vector<Theory> &theories,
                 xUnitpp::Duration maxTestRunTime, size_t maxConcurrent);

private:
    class Impl;
    std::shared_ptr<Impl> mImpl;
};

}

#endif
