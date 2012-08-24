#ifndef IOUTPUT_H_
#define IOUTPUT_H_

#include <chrono>
#include <string>

namespace xUnitpp
{

struct TestDetails;

struct IOutput
{
    virtual ~IOutput();

    virtual void ReportStart(const TestDetails &testDetails, int dataIndex) = 0;
    virtual void ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg) = 0;
    virtual void ReportSkip(const TestDetails &testDetails, const std::string &reason) = 0;
    virtual void ReportFinish(const TestDetails &testDetails, int dataIndex, std::chrono::milliseconds timeTaken) = 0;
    virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failed, std::chrono::milliseconds totalTime) = 0; 
};

}

#endif
