#ifndef OUTPUTRECORD_H_
#define OUTPUTRECORD_H_

#include <memory>
#include <mutex>
#include <tuple>
#include <utility>
#include <vector>
#include "xUnit++/IOutput.h"

namespace xUnitpp
{
    struct TestDetails;
    class TestEvent;
}

namespace xUnitpp { namespace Tests {

class OutputRecord : public IOutput
{
public:
    virtual void __stdcall ReportStart(const ITestDetails &testDetails) override;
    virtual void __stdcall ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt) override;
    virtual void __stdcall ReportSkip(const ITestDetails &testDetails, const char *reason) override;
    virtual void __stdcall ReportFinish(const ITestDetails &testDetails, long long nsTaken) override;
    virtual void __stdcall ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, long long nsTotal) override;

    std::vector<TestDetails> orderedTestList;
    std::vector<std::pair<TestDetails, TestEvent>> events;
    std::vector<std::pair<TestDetails, std::string>> skips;
    std::vector<std::pair<TestDetails, Time::Duration>> finishedTests;

    size_t summaryCount;
    size_t summarySkipped;
    size_t summaryFailed;
    Time::Duration summaryDuration;

private:
    std::mutex lock;
};

}}

#endif
