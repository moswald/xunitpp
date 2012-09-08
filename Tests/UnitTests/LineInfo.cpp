#include "xUnit++/IOutput.h"
#include "xUnit++/xUnit++.h"


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

        virtual void ReportStart(const xUnitpp::TestDetails &) override
        {
        }

        virtual void ReportFailure(const xUnitpp::TestDetails &, const std::string &, const xUnitpp::LineInfo &lineInfo) override
        {
            Assert.Equal(file, lineInfo.file);
            Assert.Equal(line, lineInfo.line);
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &, const std::string &) override
        {
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &, xUnitpp::Time::Duration) override
        {
        }

        virtual void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Time::Duration) override 
        {
        }

    private:
        std::string file;
        int line;
    } emptyReporter(file, line);


    xUnitpp::AttributeCollection attributes;
    xUnitpp::TestCollection collection;
    xUnitpp::Check localCheck;
    xUnitpp::TestCollection::Register reg(collection, test,
        "LineInfoOverridesDefaultTestLineInfo", "LineInfo", attributes,
        -1, __FILE__, __LINE__, localCheck);


}

}
