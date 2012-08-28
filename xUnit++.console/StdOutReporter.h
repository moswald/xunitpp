#ifndef STDOUTREPORTER_H_
#define STDOUTREPORTER_H_

#include "IOutput.h"

namespace xUnitpp
{

class StdOutReporter : public IOutput
{
public:
    StdOutReporter(bool verbose, bool veryVerbose);

    virtual void ReportStart(const TestDetails &, int) override;
    virtual void ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg) override;
    virtual void ReportSkip(const TestDetails &testDetails, const std::string &reason) override;
    virtual void ReportFinish(const TestDetails &, int, std::chrono::milliseconds) override;
    virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, std::chrono::milliseconds totalTime) override;

private:
    bool mVerbose;
    bool mVeryVerbose;
};

}

#endif
