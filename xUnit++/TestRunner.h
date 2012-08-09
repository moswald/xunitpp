#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include "Fact.h"
#include "Theory.h"

namespace xUnitpp
{

int RunAllTests(const std::string &suite = "", size_t maxTestRunTime = 0, size_t maxConcurrent = 0);

class TestRunner
{
public:
    TestRunner(/*std::function<void(const TestDetails &)> onTestStart,
               std::function<void(const TestDetails &, const std::string &)> onTestFailure,
               std::function<void(const TestDetails &, const std::chrono::duration<float, std::milli>)> onTestFinish,
               std::function<void(int, int, int, const std::chrono::duration<float, std::milli>)> onAllTestsComplete*/);
    int RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite, size_t maxTestRunTime = 0, size_t maxConcurrent = 0);

private:
    std::function<void(const TestDetails &)> mOnTestStart;
    std::function<void(const TestDetails &, const std::string &)> mOnTestFailure;
    std::function<void(const TestDetails &, const std::chrono::duration<float, std::milli>)> mOnTestFinish;
    std::function<void(int, int, int, const std::chrono::duration<float, std::milli>)> mOnAllTestsComplete;
};

}

#endif
