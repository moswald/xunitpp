#ifndef XMLREPORTER_H_
#define XMLREPORTER_H_

#include <map>
#include "xUnit++/IOutput.h"

namespace xUnitpp
{

class XmlReporter : public IOutput
{
public:
    XmlReporter(const std::string &filename);

    virtual void ReportStart(const TestDetails &td) override;
    virtual void ReportFailure(const TestDetails &testDetails, const std::string &msg, const LineInfo &lineInfo) override;
    virtual void ReportSkip(const TestDetails &testDetails, const std::string &reason) override;
    virtual void ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken) override;
    virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, Time::Duration totalTime) override;

public:
    struct SuiteResult;

private:
    std::string filename;
    std::map<std::string, SuiteResult> suiteResults;
};

}

#endif
