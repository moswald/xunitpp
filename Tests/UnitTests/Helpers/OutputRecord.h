#ifndef OUTPUTRECORD_H_
#define OUTPUTRECORD_H_

#include <memory>
#include <mutex>
#include <tuple>
#include <vector>
#include "xUnit++/IOutput.h"

namespace xUnitpp { namespace Tests {

class OutputRecord : public IOutput
{
public:
    virtual void ReportStart(const TestDetails &testDetails) override;
    virtual void ReportEvent(const TestDetails &testDetails, const TestEvent &evt) override;
    virtual void ReportSkip(const TestDetails &testDetails, const std::string &reason) override;
    virtual void ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken) override;
    virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, Time::Duration totalTime) override;

    std::vector<TestDetails> orderedTestList;
    std::vector<std::tuple<TestDetails, TestEvent>> events;
    std::vector<std::tuple<TestDetails, std::string>> skips;
    std::vector<std::tuple<TestDetails, Time::Duration>> finishedTests;

    size_t summaryCount;
    size_t summarySkipped;
    size_t summaryFailed;
    Time::Duration summaryDuration;

private:
    std::mutex lock;
};

}}

#endif
