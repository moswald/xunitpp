#ifndef XMLREPORTER_H_
#define XMLREPORTER_H_

#include <map>
#include "xUnit++/IOutput.h"

namespace xUnitpp { namespace Utilities
{

class XmlReporter : public IOutput
{
public:
    XmlReporter(const std::string &filename);

    virtual void _stdcall ReportStart(const ITestDetails &td) override;
    virtual void _stdcall ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt) override;
    virtual void _stdcall ReportSkip(const ITestDetails &testDetails, const char *reason) override;
    virtual void _stdcall ReportFinish(const ITestDetails &testDetails, long long nsTaken) override;
    virtual void _stdcall ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, long long nsTotal) override;

public:
    struct SuiteResult;

private:
    std::string filename;
    std::map<std::string, SuiteResult> suiteResults;
};

}}

#endif
