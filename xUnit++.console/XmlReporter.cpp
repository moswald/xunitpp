#include "XmlReporter.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include "TestDetails.h"
#include "xUnitTime.h"

namespace
{
    struct TestResult
    {
        TestResult()
        {
        }

        TestResult(const xUnitpp::TestDetails &testDetails)
            : testDetails(testDetails)
            , status(Success)
        {
        }

        xUnitpp::TestDetails testDetails;

        enum
        {
            Success,
            Failure,
            Skipped
        } status;

        std::chrono::milliseconds time;
        std::string message;
    };
}

struct xUnitpp::XmlReporter::SuiteResult
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

    std::map<std::string, TestResult> testResults;
};

namespace
{
    std::string TestName(const std::string &name, int dataIndex)
    {
        if (dataIndex < 0)
        {
            return name;
        }
        else
        {
            return name + "(" + std::to_string(dataIndex) +")";
        }
    }

    float SuiteTime(const xUnitpp::XmlReporter::SuiteResult &suiteResult)
    {
        std::chrono::milliseconds timeTaken = std::chrono::milliseconds::zero();

        for (const auto &test : suiteResult.testResults)
        {
            timeTaken += test.second.time;
        }

        return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(timeTaken).count();
    }

    float XmlTime(std::chrono::milliseconds timeTaken)
    {
        return std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(timeTaken).count();
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

    std::string XmlBeginResults(size_t tests, size_t failures, std::chrono::milliseconds totalTime)
    {
        return
            "<testsuites" +
                XmlAttribute("tests", tests) +
                XmlAttribute("failures", failures) +
                XmlAttribute("time", XmlTime(totalTime)) +
            ">\n";
    }

    std::string XmlEndResults()
    {
        return "</testsuites>\n";
    }

    std::string XmlBeginSuite(const xUnitpp::XmlReporter::SuiteResult &suite)
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
                XmlAttribute("time", test.time.count());
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

    std::string XmlTestFailed(const std::string &file, int line, const std::string &message)
    {
        return std::string("         ") +
            "<failure" +
                XmlAttribute("message", file + "(" + std::to_string(line) + "): " + XmlEscape(message)) +
            "</failure>\n";
    }

    std::string XmlTestSkipped(const std::string &message)
    {
        return std::string("         ") +
            "<skipped" +
                XmlAttribute("message", XmlEscape(message)) +
            "</skipped>\n";
    }
}

namespace xUnitpp
{

XmlReporter::XmlReporter(const std::string &filename)
    : filename(filename)
{
}

void XmlReporter::ReportAllTestsComplete(size_t testCount, size_t, size_t failureCount, std::chrono::milliseconds totalTime)
{
    auto report = [&](std::ostream &stream)
        {
            stream << XmlBeginDoc();
            stream << XmlBeginResults(testCount, failureCount, totalTime);

            for (const auto &itSuite : suiteResults)
            {
                stream << XmlBeginSuite(itSuite.second);

                for (const auto &itTest : itSuite.second.testResults)
                {
                    const auto &test = itTest.second;

                    stream << XmlBeginTest(itTest.first, test);

                    if (test.status != TestResult::Success || !test.testDetails.Attributes.empty())
                    {
                        // close <TestCase>
                        stream << ">\n";
                    }

                    for (const auto &att : test.testDetails.Attributes)
                    {
                        if (att.first != "Skip")
                        {
                            stream << XmlTestAttribute(att.first, att.second);
                        }
                    }

                    if (test.status == TestResult::Failure)
                    {
                        stream << XmlTestFailed(test.testDetails.Filename, test.testDetails.Line, test.message);
                    }
                    else if (test.status == TestResult::Skipped)
                    {
                        stream << XmlTestSkipped(test.message);
                    }

                    stream << XmlEndTest(test.status == TestResult::Success && test.testDetails.Attributes.empty());
                }

                stream << XmlEndSuite();
            }

            stream << XmlEndResults();

        };

    std::ofstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cerr << "Unable to open " << filename << " for writing.\n\n";

        report(std::cerr);
    }
    else
    {
        report(file);
    }
}

void XmlReporter::ReportStart(const TestDetails &testDetails, int dataIndex)
{
    if (suiteResults.find(testDetails.Suite) == suiteResults.end())
    {
        suiteResults.insert(std::make_pair(testDetails.Suite, SuiteResult(testDetails.Suite)));
    }

    suiteResults[testDetails.Suite].tests++;
    suiteResults[testDetails.Suite].testResults.insert(std::make_pair(TestName(testDetails.Name, dataIndex), TestResult(testDetails)));
}

void XmlReporter::ReportFailure(const TestDetails &testDetails, int dataIndex, const std::string &msg)
{
    std::string testName = TestName(testDetails.Name, dataIndex);
    suiteResults[testDetails.Suite].failures++;
    suiteResults[testDetails.Suite].testResults[testName].message = msg;
    suiteResults[testDetails.Suite].testResults[testName].status = TestResult::Failure;
}

void XmlReporter::ReportSkip(const TestDetails &testDetails, const std::string &reason)
{
    ReportStart(testDetails, -1);
    suiteResults[testDetails.Suite].skipped++;
    suiteResults[testDetails.Suite].testResults[testDetails.Name].message = reason;
    suiteResults[testDetails.Suite].testResults[testDetails.Name].status = TestResult::Skipped;
}

void XmlReporter::ReportFinish(const TestDetails &testDetails, int dataIndex, std::chrono::milliseconds timeTaken)
{
    std::string testName = TestName(testDetails.Name, dataIndex);
    suiteResults[testDetails.Suite].testResults[testName].time = timeTaken;
}

}
