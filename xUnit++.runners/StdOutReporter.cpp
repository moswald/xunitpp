#include "StdOutReporter.h"
#include <cstdio>
#include <iostream>
#include "TestDetails.h"

namespace
{
    std::string FileAndLine(const std::string &file, int line)
    {
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

void StdOutReporter::ReportStart(const TestDetails &testDetails, int dataIndex)
{
    if (mVeryVerbose)
    {
        std::cout << ("Starting test " + NameAndDataIndex(testDetails.Name, dataIndex) + ".\n");
    }
}

void StdOutReporter::ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg)
{
    std::cout << (FileAndLine(testDetails.Filename, testDetails.Line) +
        ": error in " + NameAndDataIndex(testDetails.Name, dataIndex) + ": " + msg + "\n");
}

void StdOutReporter::ReportSkip(const TestDetails &testDetails, const std::string &reason)
{
    std::cout << (FileAndLine(testDetails.Filename, testDetails.Line) +
        ": skipping " + testDetails.Name + ": " + reason + "\n");
}

void StdOutReporter::ReportFinish(const TestDetails &testDetails, int dataIndex, std::chrono::milliseconds timeTaken)
{
    if (mVerbose)
    {
        std::cout << (NameAndDataIndex(testDetails.Name, dataIndex) + ": Completed in " + (timeTaken.count() == 0 ? std::string("under 1") : std::to_string(timeTaken.count())) + "ms.\n");
    }
}

void StdOutReporter::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, std::chrono::milliseconds totalTime)
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
    std::cout << (header + std::to_string(totalTime.count()) + " milliseconds.\n");
}

}