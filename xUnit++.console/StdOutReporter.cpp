#include "StdOutReporter.h"
#include <chrono>
#include <cstdio>
#include <iostream>
#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"

namespace
{
    std::string FileAndLine(const xUnitpp::TestDetails &td, const xUnitpp::LineInfo &lineInfo)
    {
        auto file = lineInfo.file.empty() ? td.Filename : lineInfo.file;
        auto line = lineInfo.file.empty() ? td.Line : lineInfo.line;

        return file + "(" + std::to_string(line) + ")";
    }

    std::string NameAndDataIndex(const std::string &name, int dataIndex)
    {
        if (dataIndex < 0)
        {
            return name;
        }
        else
        {
            return name + "(" + std::to_string(dataIndex) +")";
        }
    }
}

namespace xUnitpp
{

StdOutReporter::StdOutReporter(bool verbose, bool veryVerbose)
    : mVerbose(verbose)
    , mVeryVerbose(veryVerbose)
{
}

void StdOutReporter::ReportStart(const TestDetails &testDetails)
{
    if (mVeryVerbose)
    {
        std::cout << ("Starting test " + testDetails.Name + ".\n");
    }
}

void StdOutReporter::ReportFailure(const TestDetails &testDetails, const std::string &msg, const LineInfo &lineInfo)
{
    std::cout << (FileAndLine(testDetails, lineInfo) +
        ": error in " + testDetails.Name+ ": " + msg + "\n");
}

void StdOutReporter::ReportSkip(const TestDetails &testDetails, const std::string &reason)
{
    std::cout << (FileAndLine(testDetails, LineInfo::empty()) +
        ": skipping " + testDetails.Name + ": " + reason + "\n");
}

void StdOutReporter::ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken)
{
    if (mVerbose)
    {
        auto ms = Time::ToMilliseconds(timeTaken);
        std::cout << (testDetails.Name + ": Completed in " + (ms.count() == 0 ? (std::to_string(timeTaken.count()) + " nanoseconds.\n") : (std::to_string(ms.count()) + " milliseconds.\n")));
    }
}

void StdOutReporter::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, Time::Duration totalTime)
{
    std::string total = std::to_string(testCount) + " tests, ";
    std::string failures = std::to_string(failureCount) + " failed, ";
    std::string skips = std::to_string(skipped) + " skipped.\n";

    std::string header;

    if (failureCount > 0)
    {
        header = "\nFAILURE: ";
    }
    else if (skipped > 0)
    {
        header = "\nWARNING: ";
    }
    else
    {
        header = "\nSuccess: ";
    }

    std::cout << (header + total + failures + skips);

    header = "Test time: ";

    auto ms = Time::ToMilliseconds(totalTime);

    if (ms.count() > 500)
    {
        std::cout << (header + std::to_string(Time::ToSeconds(totalTime).count()) + " seconds.\n");
    }
    else
    {
        std::cout << (header + std::to_string(ms.count()) + " milliseconds.\n");
    }
}

}
