#ifndef XMLREPORTER_H_
#define XMLREPORTER_H_

#include <map>
#include <ostream>
#include "xUnit++/IOutput.h"

namespace xUnitpp { namespace Utilities
{

class XmlReporter : public IOutput
{
public:
    XmlReporter(std::ostream &output);
    virtual ~XmlReporter();

    virtual void __stdcall ReportStart(const ITestDetails &td) override;
    virtual void __stdcall ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt) override;
    virtual void __stdcall ReportSkip(const ITestDetails &testDetails, const char *reason) override;
    virtual void __stdcall ReportFinish(const ITestDetails &testDetails, long long nsTaken) override;
    virtual void __stdcall ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, long long nsTotal) override;

public:
    struct SuiteResult;

private:
    XmlReporter &operator =(XmlReporter) /* = delete; */;

private:
    std::ostream &output;
    std::map<std::string, SuiteResult> suiteResults;
};

}}

#endif
