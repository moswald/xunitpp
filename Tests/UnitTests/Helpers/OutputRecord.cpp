#include "OutputRecord.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"
#include "xUnit++/TestEvent.h"

namespace xUnitpp { namespace Tests {

void OutputRecord::ReportStart(const TestDetails &testDetails)
{
    std::lock_guard<std::mutex> guard(lock);
    orderedTestList.push_back(testDetails);
}

void OutputRecord::ReportEvent(const TestDetails &testDetails, const TestEvent &evt)
{
    std::lock_guard<std::mutex> guard(lock);
    events.push_back(std::make_tuple(testDetails, evt));
}

void OutputRecord::ReportSkip(const TestDetails &testDetails, const std::string &reason)
{
    std::lock_guard<std::mutex> guard(lock);
    skips.push_back(std::make_tuple(testDetails, reason));
}

void OutputRecord::ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken)
{
    std::lock_guard<std::mutex> guard(lock);
    finishedTests.push_back(std::make_tuple(testDetails, timeTaken));
}

void OutputRecord::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, Time::Duration totalTime)
{
    summaryCount = testCount;
    summarySkipped = skipped;
    summaryFailed = failed;
    summaryDuration = totalTime;
}

}}
