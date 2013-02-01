#include "XmlReporter.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>
#include "xUnit++/ITestDetails.h"
#include "xUnit++/ITestEvent.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/xUnitTime.h"

namespace
{
    struct TestResult
    {
        TestResult(const xUnitpp::ITestDetails &testDetails)
            : testDetails(testDetails)
            , status(Success)
        {
        }

        const xUnitpp::ITestDetails &testDetails;

        enum
        {
            Success,
            Failure,
            Skipped
        } status;

        xUnitpp::Time::Duration time;
        std::vector<std::string> messages;

    private:
        TestResult &operator =(TestResult other) /* = delete */;
    };
}

struct xUnitpp::Utilities::XmlReporter::SuiteResult
{
    SuiteResult()
    {
    }

    SuiteResult(const std::string &name)
        : name(name)
        , tests(0)
        , failures(0)
        , skipped(0)
    {
    }

    std::string name;
    int tests;
    int failures;
    int skipped;

    std::vector<TestResult> testResults;
};

namespace
{
    TestResult &GetTestResult(std::vector<TestResult> &testResults, const std::string &fullName)
    {
        // if fullName isn't found it's a bug: go ahead and crash :P
        return *std::find_if(testResults.begin(), testResults.end(),
            [&](const TestResult &test)
            {
                return test.testDetails.GetFullName() == fullName;
            });
    }

    float SuiteTime(const xUnitpp::Utilities::XmlReporter::SuiteResult &suiteResult)
    {
        xUnitpp::Time::Duration timeTaken = xUnitpp::Time::Duration::zero();

        for (const auto &test : suiteResult.testResults)
        {
            timeTaken += test.time;
        }

        return xUnitpp::Time::ToSeconds(timeTaken).count();
    }

    void ReplaceChar(std::string &str, char c, const std::string &replacement)
    {
        for (size_t pos = str.find(c); pos != std::string::npos; pos = str.find(c, pos + 1))
        {
            str.replace(pos, 1, replacement);
        }
    }

    std::string XmlEscape(const std::string &value)
    {
        std::string escaped = value;

        ReplaceChar(escaped, '&', "&amp;");
        ReplaceChar(escaped, '<', "&lt;");
        ReplaceChar(escaped, '>', "&gt;");
        ReplaceChar(escaped, '\'', "&apos;");
        ReplaceChar(escaped, '\"', "&quot;");

        return escaped;
    }

    std::string XmlAttribute(const std::string &name, const std::string &value)
    {
        return " " + name + "=\"" + value + "\"";
    }

    std::string XmlAttribute(const std::string &name, const char *value)
    {
        return XmlAttribute(name, std::string(value));
    }

    template<typename T>
    std::string XmlAttribute(const std::string &name, T value)
    {
        return XmlAttribute(name, std::to_string(value));
    }

    std::string XmlBeginDoc()
    {
        return
            "<?xml" +
                XmlAttribute("version", "1.0") +
                XmlAttribute("encoding", "utf-8") +
            " ?>\n";
    }

    std::string XmlBeginResults(size_t tests, size_t failures, long long nsTotal)
    {
        xUnitpp::Time::Duration totalTime(nsTotal);
        return
            "<testsuites" +
                XmlAttribute("tests", tests) +
                XmlAttribute("failures", failures) +
                XmlAttribute("time", xUnitpp::Time::ToSeconds(totalTime).count()) +
            ">\n";
    }

    std::string XmlEndResults()
    {
        return "</testsuites>\n";
    }

    std::string XmlBeginSuite(const xUnitpp::Utilities::XmlReporter::SuiteResult &suite)
    {
        return std::string("   ") +
            "<testsuite" +
                XmlAttribute("name", suite.name) +
                XmlAttribute("tests", suite.tests) +
                XmlAttribute("failures", suite.failures) +
                XmlAttribute("skipped", suite.skipped) +
                XmlAttribute("time", SuiteTime(suite)) +
            ">\n";
    }

    std::string XmlEndSuite()
    {
        return std::string("   ") +
            "</testsuite>\n";
    }

    std::string XmlBeginTest(const std::string &testName, const TestResult &test)
    {
        return std::string("      ") +
            "<testcase" +
                XmlAttribute("name", testName) +
                XmlAttribute("time", xUnitpp::Time::ToSeconds(test.time).count());
    }

    std::string XmlEndTest(bool singleTag)
    {
        if (singleTag)
        {
            return " />\n";
        }

        return std::string("      ") +
            "</testcase>\n";
    }

    std::string XmlTestAttribute(const std::string &name, const std::string &value)
    {
        return std::string("         ") +
            "<property" +
                XmlAttribute("name", name) +
                XmlAttribute("value", value) +
            " />\n";
    }

    std::string XmlTestFailed(const std::string &fileAndLine, const std::vector<std::string> &messages)
    {
        std::string result;
        for (const auto &message : messages)
        {
            result += std::string("         ") +
                "<failure" +
                    XmlAttribute("message", fileAndLine + ": " + XmlEscape(message)) +
                " />\n";
        }

        return result;
    }

    std::string XmlTestSkipped(const std::string &message)
    {
        return std::string("         ") +
            "<skipped" +
                XmlAttribute("message", XmlEscape(message)) +
            "</skipped>\n";
    }
}

namespace xUnitpp { namespace Utilities
{

XmlReporter::XmlReporter(std::ostream &output)
    : output(output)
{
}

XmlReporter::~XmlReporter()
{
}

void XmlReporter::ReportAllTestsComplete(size_t testCount, size_t, size_t failureCount, long long nsTotal)
{
    output << XmlBeginDoc();
    output << XmlBeginResults(testCount, failureCount, nsTotal);

    for (const auto &itSuite : suiteResults)
    {
        output << XmlBeginSuite(itSuite.second);

        for (const auto &test : itSuite.second.testResults)
        {
            output << XmlBeginTest(test.testDetails.GetFullName(), test);

            if (test.status != TestResult::Success || test.testDetails.GetAttributeCount() != 0)
            {
                // close <TestCase>
                output << ">\n";
            }

            for (auto i = 0U; i != test.testDetails.GetAttributeCount(); ++i)
            {
                std::string key = test.testDetails.GetAttributeKey(i);
                if (key != "Skip")
                {
                    std::string value = test.testDetails.GetAttributeValue(i);

                    output << XmlTestAttribute(key, value);
                }
            }

            if (test.status == TestResult::Failure)
            {
                xUnitpp::LineInfo li(test.testDetails.GetFile(), test.testDetails.GetLine());
                output << XmlTestFailed(to_string(li), test.messages);
            }
            else if (test.status == TestResult::Skipped)
            {
                output << XmlTestSkipped(test.messages[0]);
            }

            output << XmlEndTest(test.status == TestResult::Success && test.testDetails.GetAttributeCount() == 0);
        }

        output << XmlEndSuite();
    }

    output << XmlEndResults();
}

void XmlReporter::ReportStart(const ITestDetails &testDetails)
{
    std::string suite = testDetails.GetSuite();
    if (suiteResults.find(suite) == suiteResults.end())
    {
        suiteResults.insert(std::make_pair(suite, SuiteResult(suite)));
    }

    suiteResults[suite].tests++;
    suiteResults[suite].testResults.push_back(TestResult(testDetails));
}

void XmlReporter::ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt)
{
    if (evt.GetIsFailure())
    {
        std::string suite = testDetails.GetSuite();
        std::string name = testDetails.GetFullName();
        
        suiteResults[suite].failures++;

        auto &testResult = GetTestResult(suiteResults[suite].testResults, name);
        testResult.messages.push_back(evt.GetToString());
        testResult.status = TestResult::Failure;
    }
}

void XmlReporter::ReportSkip(const ITestDetails &testDetails, const char *reason)
{
    ReportStart(testDetails);

    std::string suite = testDetails.GetSuite();
    std::string name = testDetails.GetFullName();

    suiteResults[suite].skipped++;

    auto &testResult = GetTestResult(suiteResults[suite].testResults, name);
    testResult.messages.push_back(reason);
    testResult.status = TestResult::Skipped;
}

void XmlReporter::ReportFinish(const ITestDetails &testDetails, long long nsTaken)
{
    GetTestResult(suiteResults[testDetails.GetSuite()].testResults, testDetails.GetFullName()).time = Time::Duration(nsTaken);
}

}}
