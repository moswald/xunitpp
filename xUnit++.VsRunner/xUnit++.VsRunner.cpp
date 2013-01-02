#include "stdafx.h"
#include "xUnit++.VsRunner.h"
#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <msclr/marshal_cppstd.h>
#include "xUnit++/EventLevel.h"
#include "xUnit++/ExportApi.h"
#include "xUnit++/IOutput.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/ITestDetails.h"
#include "xUnit++/ITestEvent.h"
#include "TestAssembly.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestPlatform::ObjectModel;
using namespace Microsoft::VisualStudio::TestPlatform::ObjectModel::Adapter;
using namespace Microsoft::VisualStudio::TestPlatform::ObjectModel::Logging;
using namespace msclr::interop;

namespace
{
    String ^TestKey(const xUnitpp::ITestDetails &td)
    {
        return marshal_as<String ^>(td.GetFullName());
    }

    std::string DisplayName(const xUnitpp::ITestDetails &td)
    {
        std::string name = td.GetName();
        if (!std::string(td.GetParams()).empty())
        {
            name += " [" + std::to_string(td.GetTestInstance()) + "]";
        }

        return name;
    }

    String ^TestName(const xUnitpp::ITestDetails &td)
    {
        return marshal_as<String ^>(DisplayName(td));
    }

    ref class ManagedReporter
    {
    public:
        ManagedReporter(ITestExecutionRecorder ^recorder, const std::vector<gcroot<TestCase ^>> &testCases)
            : recorder(recorder)
        {
            for (auto &test : testCases)
            {
                if (!this->testCases.ContainsKey(test->FullyQualifiedName))
                {
                    this->testCases.Add(test->FullyQualifiedName, test);
                }
            }
        }

        void ReportStart(const xUnitpp::ITestDetails &td)
        {
            auto key = TestKey(td);
            auto name = TestName(td);
            recorder->RecordStart(testCases[key]);

            auto result = gcnew TestResult(testCases[key]);
            result->ComputerName = Environment::MachineName;
            result->DisplayName = name;
            result->Outcome = TestOutcome::None;

            testResults.Add(key, result);
        }

        void ReportEvent(const xUnitpp::ITestDetails &td, const xUnitpp::ITestEvent &evt)
        {
            auto key = TestKey(td);
            auto result = testResults[key];

            if (evt.GetIsFailure())
            {
                result->Outcome = TestOutcome::Failed;
            }

            result->Messages->Add(gcnew TestResultMessage(marshal_as<String ^>(to_string(evt.GetLevel())), marshal_as<String ^>(evt.GetToString())));
        }

        void ReportSkip(const xUnitpp::ITestDetails &td, const std::string &)
        {
            auto key = TestKey(td);
            auto testCase = testCases[key];
            auto result = gcnew TestResult(testCase);
            result->ComputerName = Environment::MachineName;
            result->DisplayName = TestName(td);
            result->Duration = TimeSpan::FromSeconds(0);
            result->Outcome = TestOutcome::Skipped;
            recorder->RecordEnd(testCase, result->Outcome);
            recorder->RecordResult(result);
        }

        void ReportFinish(const xUnitpp::ITestDetails &td, xUnitpp::Time::Duration timeTaken)
        {
            auto key = TestKey(td);
            auto result = testResults[key];
            result->Duration = TimeSpan::FromSeconds(xUnitpp::Time::ToSeconds(timeTaken).count());

            if (result->Outcome == TestOutcome::None)
            {
                result->Outcome = TestOutcome::Passed;
            }

            recorder->RecordEnd(testCases[key], result->Outcome);
            recorder->RecordResult(result);
        }

        void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Time::Duration)
        {
        }

    private:
        ITestExecutionRecorder ^recorder;
        Dictionary<String ^, TestCase ^> testCases;
        Dictionary<String ^, TestResult ^> testResults;
    };

    struct NativeReporter : xUnitpp::IOutput
    {
        NativeReporter(ITestExecutionRecorder ^recorder, const std::vector<gcroot<TestCase ^>> &testCases)
            : reporter(gcnew ManagedReporter(recorder, testCases))
        {
        }

        virtual void __stdcall ReportStart(const xUnitpp::ITestDetails &td) override
        {
            reporter->ReportStart(td);
        }

        virtual void __stdcall ReportEvent(const xUnitpp::ITestDetails &testDetails, const xUnitpp::ITestEvent &evt) override
        {
            reporter->ReportEvent(testDetails, evt);
        }

        virtual void __stdcall ReportSkip(const xUnitpp::ITestDetails &testDetails, const char *reason) override
        {
            reporter->ReportSkip(testDetails, reason);
        }

        virtual void __stdcall ReportFinish(const xUnitpp::ITestDetails &testDetails, long long nsTaken) override
        {
            reporter->ReportFinish(testDetails, xUnitpp::Time::Duration(nsTaken));
        }

        virtual void __stdcall ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, long long nsTotal) override
        {
            reporter->ReportAllTestsComplete(testCount, skipped, failureCount, xUnitpp::Time::Duration(nsTotal));
        }

    private:
        gcroot<ManagedReporter ^> reporter;
    };

    class ManagedTestAssembly : public xUnitpp::Utilities::TestAssembly
    {
    public:
        ManagedTestAssembly(const std::string &file)
            : TestAssembly(file, true)
        {
        }

        ~ManagedTestAssembly()
        {
        }

        void AddTestCase(TestCase ^test)
        {
            tests.push_back(gcroot<TestCase ^>(test));
        }

        void RunFilteredTests(ITestExecutionRecorder ^recorder, bool &cancelled)
        {
            NativeReporter reporter(recorder, tests);
            FilteredTestsRunner(0, 0, reporter,
                [&](const xUnitpp::ITestDetails &testDetails)
                {
                    return !cancelled && std::find_if(tests.begin(), tests.end(),
                        [&](gcroot<TestCase ^> test)
                        {
                            return test->DisplayName == TestName(testDetails);
                        }) != tests.end();
                });
        }

    private:
        std::vector<gcroot<TestCase ^>> tests;
    };

    IEnumerable<TestCase ^> ^SingleSourceTestCases(String ^_source, Uri ^_uri)
    {
        auto results = gcnew List<TestCase ^>();
        auto source = marshal_as<std::string>(_source);

        if (auto assembly = ManagedTestAssembly(source))
        {
            struct testDetails
            {
                std::string fullName;
                std::string name;
                std::string file;
                int line;
            };

            std::vector<testDetails> tests;

            assembly.EnumerateTestDetails(
                [&](const xUnitpp::ITestDetails &td)
                {
                    testDetails testDetails = { td.GetFullName(), DisplayName(td), td.GetFile(), td.GetLine() };
                    tests.push_back(testDetails);
                });

            auto uri = gcroot<Uri ^>(_uri);

            for (const auto &test : tests)
            {
                TestCase ^testCase = gcnew TestCase(marshal_as<String ^>(test.fullName), uri, marshal_as<String ^>(source));
                testCase->DisplayName = marshal_as<String ^>(test.name);
                testCase->CodeFilePath = marshal_as<String ^>(test.file);
                testCase->LineNumber = test.line;

                results->Add(testCase);
            }
        }

        return results;
    }
}

namespace xUnitpp { namespace VsRunner
{

xUnitppVsRunner::xUnitppVsRunner()
    : mUri(gcnew Uri(VSRUNNER_URI))
    , mCancelled(false)
{
}

void xUnitppVsRunner::DiscoverTests(IEnumerable<String ^> ^sources, IDiscoveryContext ^, IMessageLogger ^, ITestCaseDiscoverySink ^discoverySink)
{
    for each (String ^source in sources)
    {
        for each (TestCase ^test in SingleSourceTestCases(source, mUri))
        {
            discoverySink->SendTestCase(test);
        }
    }
}

void xUnitppVsRunner::RunTests(IEnumerable<String ^> ^sources, IRunContext ^, IFrameworkHandle ^framework)
{
    mCancelled = false;

    for each (String ^source in sources)
    {
        if (RunTests(SingleSourceTestCases(source, mUri), framework))
        {
            // cancelled
            break;
        }
    }
}

void xUnitppVsRunner::RunTests(IEnumerable<TestCase ^> ^tests, IRunContext ^, IFrameworkHandle ^framework)
{
    mCancelled = false;
    RunTests(tests, framework);
}

void xUnitppVsRunner::Cancel()
{
    mCancelled = true;
}

bool xUnitppVsRunner::RunTests(IEnumerable<TestCase ^> ^tests, ITestExecutionRecorder ^recorder)
{
    std::map<std::string, std::shared_ptr<ManagedTestAssembly>> assemblies;

    for each (TestCase ^test in tests)
    {
        auto source = marshal_as<std::string>(test->Source);
        if (assemblies.find(source) == assemblies.end())
        {
            if (auto assembly = std::make_shared<ManagedTestAssembly>(source))
            {
                assemblies.insert(std::make_pair(source, assembly));
            }
            else
            {
                recorder->SendMessage(TestMessageLevel::Warning,
                    String::Format("Failed to reload assembly {0} during test run.", test->Source));
            }
        }

        auto it = assemblies.find(source);
        if (it != assemblies.end())
        {
            it->second->AddTestCase(test);
        }
    }

    for (auto it = assemblies.begin(); it != assemblies.end() && !mCancelled; ++it)
    {
        auto &assembly = it->second;
        pin_ptr<bool> cancelled(&mCancelled);
        assembly->RunFilteredTests(recorder, *cancelled);
    }

    return mCancelled;
}

}}
