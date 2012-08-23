#include "DefaultReporter.h"
#include <cstdio>
#include <iostream>
#include "TestDetails.h"

namespace xUnitpp
{

namespace DefaultReporter
{
    void ReportStart(const TestDetails &, int)
    {
    }

    void ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg)
    {
        if (dataIndex < 0)
        {
            std::cerr << (testDetails.Filename + "(" + std::to_string(testDetails.Line) +
                "): error in " + testDetails.Name + ": " + msg + "\n");
        }
        else
        {
            std::cerr << (testDetails.Filename + "(" + std::to_string(testDetails.Line) +
                "): error in " + testDetails.Name + "(" + std::to_string(dataIndex) + "): " + msg + "\n");
        }
    }

    void ReportSkip(const TestDetails &testDetails, const std::string &reason)
    {
        std::cerr << (testDetails.Filename + "(" + std::to_string(testDetails.Line) +
            "): skipping " + testDetails.Name + ": " + reason + "\n");
    }

    void ReportFinish(const TestDetails &, int, std::chrono::milliseconds)
    {
    }

    void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, std::chrono::milliseconds totalTime)
    {
        std::string total = std::to_string(testCount) + " tests, ";
        std::string failures = std::to_string(failureCount) + " failed, ";
        std::string skips = std::to_string(skipped) + " skipped.\n";

        std::string header;

        if (failureCount > 0)
        {
            header = "FAILURE: ";
        }
        else if (skipped > 0)
        {
            header = "WARNING: ";
        }
        else
        {
            header = "Success: ";
        }

        std::cout << (header + total + failures + skips);

        header = "Test time: ";
        std::cout << (header + std::to_string(totalTime.count()) + " milliseconds.\n");
    }

}

}
