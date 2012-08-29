#ifndef IOUTPUT_H_
#define IOUTPUT_H_

#include <string>
#include "xUnitTime.h"

namespace xUnitpp
{

struct LineInfo;
struct TestDetails;

struct IOutput
{
    virtual ~IOutput();

    virtual void ReportStart(const TestDetails &testDetails, int dataIndex) = 0;
    virtual void ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg, const LineInfo &lineInfo) = 0;
    virtual void ReportSkip(const TestDetails &testDetails, const std::string &reason) = 0;
    virtual void ReportFinish(const TestDetails &testDetails, int dataIndex, xUnitpp::Duration timeTaken) = 0;
    virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, xUnitpp::Duration totalTime) = 0; 
};

}

#endif
