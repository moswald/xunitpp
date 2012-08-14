#include "DefaultReporter.h"
#include <cstdio>
#include <iostream>
#include "TestDetails.h"

namespace xUnitpp
{

namespace DefaultReporter
{
    void ReportStart(const TestDetails &)
    {
    }

    void ReportFailure(const TestDetails &testDetails, const std::string &msg)
    {
        std::cerr << (testDetails.Filename + "(" + std::to_string(testDetails.Line) +
            "): error " + testDetails.Name + ": " + msg + "\n");
    }

    void ReportFinish(const TestDetails &, std::chrono::milliseconds)
    {
    }

    void ReportAllTestsComplete(size_t testCount, size_t failureCount, size_t skipped, std::chrono::milliseconds totalTime)
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
