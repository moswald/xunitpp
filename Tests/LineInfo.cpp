#include "xUnit++.h"
#include "IOutput.h"

using xUnitpp::Assert;

SUITE(LineInfo)
{

FACT(LineInfoOverridesDefaultTestLineInfo)
{
    auto file = "filename";
    auto line = 1;
    auto test = [=]() { Assert.Fail(xUnitpp::LineInfo(file, line)); };

    struct EmptyReporter : xUnitpp::IOutput
    {
        EmptyReporter(const std::string &file, int line)
            : file(file)
            , line(line)
        {
        }

        virtual void ReportStart(const xUnitpp::TestDetails &, int) override
        {
        }

        virtual void ReportFailure(const xUnitpp::TestDetails &, int, const std::string &, const xUnitpp::LineInfo &lineInfo) override
        {
            Assert.Equal(file, lineInfo.file);
            Assert.Equal(line, lineInfo.line);
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &, const std::string &) override
        {
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &, int, xUnitpp::Duration) override
        {
        }

        virtual void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Duration) override 
        {
        }

    private:
        std::string file;
        int line;
    } emptyReporter(file, line);


    xUnitpp::AttributeCollection attributes;
    xUnitpp::TestCollection collection;
    xUnitpp::TestCollection::Register reg(collection, test, "LineInfoOverridesDefaultTestLineInfo", "LineInfo", attributes, -1, __FILE__, __LINE__);


}

}
