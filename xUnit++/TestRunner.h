#ifndef TESTRUNNER_H_
#define TESTRUNNER_H_

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
    //TestRunner(std::function<void(const TestDetails &testDetail)> onTestStart, 
    int RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite, size_t maxTestRunTime = 0, size_t maxConcurrent = 0);
};

}

#endif
