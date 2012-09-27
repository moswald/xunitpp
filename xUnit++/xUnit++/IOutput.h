#ifndef IOUTPUT_H_
#define IOUTPUT_H_

#include <string>
#include "xUnitTime.h"

namespace xUnitpp
{

struct LineInfo;
struct TestDetails;
class TestEvent;

struct IOutput
{
    virtual ~IOutput();

    virtual void ReportStart(const TestDetails &testDetails) = 0;
    virtual void ReportEvent(const TestDetails &testDetails, const TestEvent &evt) = 0;
    virtual void ReportSkip(const TestDetails &testDetails, const std::string &reason) = 0;
    virtual void ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken) = 0;
    virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, Time::Duration totalTime) = 0; 
};

}

#endif
