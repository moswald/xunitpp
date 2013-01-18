#ifndef CONSOLEREPORTER_H_
#define CONSOLEREPORTER_H_

#include <memory>
#include "xUnit++/IOutput.h"

namespace xUnitpp
{

class ConsoleReporter : public IOutput
{
public:
    ConsoleReporter(bool verbose, bool sort, bool group);
    virtual ~ConsoleReporter();

    virtual void __stdcall ReportStart(const ITestDetails &) override;
    virtual void __stdcall ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt) override;
    virtual void __stdcall ReportSkip(const ITestDetails &testDetails, const char *reason) override;
    virtual void __stdcall ReportFinish(const ITestDetails &, long long nsTaken) override;
    virtual void __stdcall ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, long long nsTotal) override;

private:
    class ReportCache;
    std::unique_ptr<ReportCache> cache;
};

}

#endif
