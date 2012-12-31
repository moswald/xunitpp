#include "OutputRecord.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"
#include "xUnit++/TestEvent.h"

namespace xUnitpp { namespace Tests {

void OutputRecord::ReportStart(const ITestDetails &testDetails)
{
    std::lock_guard<std::mutex> guard(lock);
    orderedTestList.push_back(static_cast<const TestDetails &>(testDetails));
}

void OutputRecord::ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt)
{
    std::lock_guard<std::mutex> guard(lock);
    events.push_back(std::make_pair(static_cast<const TestDetails &>(testDetails), static_cast<const TestEvent &>(evt)));
}

void OutputRecord::ReportSkip(const ITestDetails &testDetails, const char *reason)
{
    std::lock_guard<std::mutex> guard(lock);
    skips.push_back(std::make_pair(static_cast<const TestDetails &>(testDetails), reason));
}

void OutputRecord::ReportFinish(const ITestDetails &testDetails, long long nsTaken)
{
    std::lock_guard<std::mutex> guard(lock);
    finishedTests.push_back(std::make_pair(static_cast<const TestDetails &>(testDetails), Time::Duration(nsTaken)));
}

void OutputRecord::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, long long nsTotal)
{
    summaryCount = testCount;
    summarySkipped = skipped;
    summaryFailed = failed;
    summaryDuration = Time::Duration(nsTotal);
}

}}
