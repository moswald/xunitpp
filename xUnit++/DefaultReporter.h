#ifndef DEFAULTREPORTER_H_
#define DEFAULTREPORTER_H_

#include <string>
#include "xUnitTime.h"

namespace xUnitpp
{

struct TestDetails;

namespace DefaultReporter
{
    void ReportStart(const TestDetails &);
    void ReportFailure(const TestDetails &testDetails, const std::string &msg);
    void ReportFinish(const TestDetails &, milliseconds);
    void ReportAllTestsComplete(size_t testCount, size_t failureCount, size_t skipped, milliseconds totalTime); 
}

}

#endif
