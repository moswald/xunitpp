#ifndef XUNITTESTRUNNER_H_
#define XUNITTESTRUNNER_H_

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace xUnitpp
{

class Fact;
struct IOutput;
struct TestDetails;
class Theory;

typedef int(*FilteredTestsRunner)(int, std::shared_ptr<IOutput>, std::function<bool(const TestDetails &)>);
int RunFilteredTests(int timeLimit, std::shared_ptr<xUnitpp::IOutput> testReporter, std::function<bool(const xUnitpp::TestDetails &)> filter);

class TestRunner
{
public:
    TestRunner(std::shared_ptr<IOutput> testReporter);
    int RunTests(const std::vector<Fact> &facts, const std::vector<Theory> &theories, const std::string &suite,
                 std::chrono::milliseconds maxTestRunTime = std::chrono::milliseconds::zero(), size_t maxConcurrent = 0);

private:
    class Impl;
    std::unique_ptr<Impl> mImpl;
};

}

#endif
