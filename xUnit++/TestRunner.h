#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Fact.h"
#include "Theory.h"
#include "xUnitTime.h"

namespace xUnitpp
{

size_t RunAllTests(const std::string &suite = "", size_t maxTestRunTime = 0, size_t maxConcurrent = 0);

class TestRunner
{
public:
    TestRunner(std::function<void(const TestDetails &)> onTestStart,
               std::function<void(const TestDetails &, const std::string &)> onTestFailure,
               std::function<void(const TestDetails &, milliseconds)> onTestFinish,
               std::function<void(int, int, int, milliseconds)> onAllTestsComplete);
    size_t RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite, size_t maxTestRunTime = 0, size_t maxConcurrent = 0);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}

#endif
