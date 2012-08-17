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
class Theory;
struct TestDetails;

size_t RunAllTests(const std::string &suite);
size_t RunAllTests(std::chrono::milliseconds maxTestRunTime);
size_t RunAllTests(const std::string &suite = "", std::chrono::milliseconds maxTestRunTime = std::chrono::milliseconds::zero(), size_t maxConcurrent = 0);

class TestRunner
{
public:
    TestRunner(std::function<void(const TestDetails &, int)> onTestStart,
               std::function<void(const TestDetails &, int, const std::string &)> onTestFailure,
               std::function<void(const TestDetails &, int, std::chrono::milliseconds)> onTestFinish,
               std::function<void(int, int, int, std::chrono::milliseconds)> onAllTestsComplete);
    size_t RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite,
                    std::chrono::milliseconds maxTestRunTime = std::chrono::milliseconds::zero(), size_t maxConcurrent = 0);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}

#endif
