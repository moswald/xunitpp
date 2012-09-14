#include "OutputRecord.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"

namespace xUnitpp { namespace Tests {

void OutputRecord::ReportStart(const TestDetails &testDetails)
{
    std::lock_guard<std::mutex> guard(lock);
    orderedTestList.push_back(testDetails);
}

void OutputRecord::ReportFailure(const TestDetails &testDetails, const std::string &msg, const LineInfo &lineInfo)
{
    std::lock_guard<std::mutex> guard(lock);
    failures.push_back(std::make_tuple(testDetails, msg, lineInfo));
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
