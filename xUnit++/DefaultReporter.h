#ifndef DEFAULTREPORTER_H_
#define DEFAULTREPORTER_H_

#include <string>
#include "xUnitTime.h"

namespace xUnitpp
{

struct TestDetails;

namespace DefaultReporter
{
    void ReportStart(const TestDetails &, int);
    void ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg);
    void ReportFinish(const TestDetails &, int, std::chrono::milliseconds);
    void ReportAllTestsComplete(size_t testCount, size_t failureCount, size_t skipped, std::chrono::milliseconds totalTime); 
}

}

#endif
