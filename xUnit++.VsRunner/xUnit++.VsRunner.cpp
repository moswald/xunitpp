#include "stdafx.h"
#include "xUnit++.VsRunner.h"
#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <msclr/marshal_cppstd.h>
#include "xUnit++/ExportApi.h"
#include "xUnit++/IOutput.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"
#include "xUnit++/TestEvent.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace Microsoft::VisualStudio::TestPlatform::ObjectModel;
using namespace Microsoft::VisualStudio::TestPlatform::ObjectModel::Adapter;
using namespace Microsoft::VisualStudio::TestPlatform::ObjectModel::Logging;
using namespace msclr::interop;

namespace
{
    ref struct ManagedReporter
    {
        ManagedReporter(ITestExecutionRecorder ^recorder, const std::vector<gcroot<TestCase ^>> &testCases)
            : recorder(recorder)
        {
            for (auto &test : testCases)
            {
                if (!this->testCases.ContainsKey(test->DisplayName))
                this->testCases.Add(test->DisplayName, test);
            }
        }

        void ReportStart(const xUnitpp::TestDetails &td)
        {
            auto key = marshal_as<String ^>(td.Name);
            auto name = marshal_as<String ^>(td.ShortName);
            recorder->RecordStart(testCases[name]);

            auto result = gcnew TestResult(testCases[name]);
            result->ComputerName = Environment::MachineName;
            result->DisplayName = name;
            result->Outcome = TestOutcome::None;

            testResults.Add(key, result);
        }

        void ReportEvent(const xUnitpp::TestDetails &td, const xUnitpp::TestEvent &evt)
        {
            auto result = testResults[marshal_as<String ^>(td.Name)];

            if (evt.IsFailure())
            {
                result->Outcome = TestOutcome::Failed;
            }

            result->Messages->Add(gcnew TestResultMessage(marshal_as<String ^>(to_string(evt.Level())), marshal_as<String ^>(to_string(evt))));
        }

        void ReportSkip(const xUnitpp::TestDetails &td, const std::string &)
        {
            auto testCase = testCases[marshal_as<String ^>(td.ShortName)];
            auto result = gcnew TestResult(testCase);
            result->ComputerName = Environment::MachineName;
            result->DisplayName = marshal_as<String ^>(td.ShortName);
            result->Duration = TimeSpan::FromSeconds(0);
            result->Outcome = TestOutcome::Skipped;
            recorder->RecordEnd(testCase, result->Outcome);
            recorder->RecordResult(result);
        }

        void ReportFinish(const xUnitpp::TestDetails &td, xUnitpp::Time::Duration timeTaken)
        {
            auto result = testResults[marshal_as<String ^>(td.Name)];
            result->Duration = TimeSpan::FromSeconds(xUnitpp::Time::ToSeconds(timeTaken).count());

            if (result->Outcome == TestOutcome::None)
            {
                result->Outcome = TestOutcome::Passed;
            }

            recorder->RecordEnd(testCases[marshal_as<String ^>(td.ShortName)], result->Outcome);
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

        virtual void ReportStart(const xUnitpp::TestDetails &td) override
        {
            reporter->ReportStart(td);
        }

        virtual void ReportEvent(const xUnitpp::TestDetails &testDetails, const xUnitpp::TestEvent &evt) override
        {
            reporter->ReportEvent(testDetails, evt);
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &testDetails, const std::string &reason) override
        {
            reporter->ReportSkip(testDetails, reason);
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &testDetails, xUnitpp::Time::Duration timeTaken) override
        {
            reporter->ReportFinish(testDetails, timeTaken);
        }

        virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, xUnitpp::Time::Duration totalTime) override
        {
            reporter->ReportAllTestsComplete(testCount, skipped, failureCount, totalTime);
        }

    private:
        gcroot<ManagedReporter ^> reporter;
    };

    class TestAssembly
    {
    private:
        typedef bool (TestAssembly::*bool_type)() const;
        bool is_valid() const
        {
            return EnumerateTestDetails != nullptr && FilteredTestsRunner != nullptr;
        }

    public:
        TestAssembly(const std::string &file)
            : EnumerateTestDetails(nullptr)
            , FilteredTestsRunner(nullptr)
            , module(nullptr)
        {
            char tempPath[MAX_PATH] = {0};
            if (GetTempPath(MAX_PATH, tempPath) > 0)
            {
                if (GetTempFileName(tempPath, "xU+", 0, tempPath) != 0)
                {
                    if (CopyFile(file.c_str(), tempPath, FALSE))
                    {
                        tempFile = tempPath;
                        if ((module = LoadLibrary(tempPath)) != nullptr)
                        {
                            EnumerateTestDetails = (xUnitpp::EnumerateTestDetails)GetProcAddress(module, "EnumerateTestDetails");
                            FilteredTestsRunner = (xUnitpp::FilteredTestsRunner)GetProcAddress(module, "FilteredTestsRunner");
                        }
                    }
                }
            }
        }

        ~TestAssembly()
        {
            if (module)
            {
                FreeLibrary(module);
            }

            if (!tempFile.empty())
            {
                DeleteFile(tempFile.c_str());
            }
        }

        // !!!VS enable this when Visual Studio supports it
        //explicit operator bool() const
        //{
        //    return module != nullptr;
        //}

        operator bool_type() const
        {
            return is_valid() ? &TestAssembly::is_valid : nullptr;
        }

        xUnitpp::EnumerateTestDetails EnumerateTestDetails;

        void AddTestCase(TestCase ^test)
        {
            tests.push_back(gcroot<TestCase ^>(test));
        }

        void RunFilteredTests(ITestExecutionRecorder ^recorder, bool &cancelled)
        {
            NativeReporter reporter(recorder, tests);
            FilteredTestsRunner(0, 0, reporter,
                [&](const xUnitpp::TestDetails &testDetails)
                {
                    return !cancelled && std::find_if(tests.begin(), tests.end(),
                        [&](gcroot<TestCase ^> test)
                        {
                            return marshal_as<std::string>(test->DisplayName) == testDetails.ShortName;
                        }) != tests.end();
                });
        }

    private:
        xUnitpp::FilteredTestsRunner FilteredTestsRunner;
        std::string tempFile;
        HMODULE module;
        std::vector<gcroot<TestCase ^>> tests;
    };

    IEnumerable<TestCase ^> ^SingleSourceTestCases(String ^_source, Uri ^_uri)
    {
        auto result = gcnew Dictionary<String ^, TestCase ^>();

        auto source = marshal_as<std::string>(_source);
        if (auto assembly = TestAssembly(source))
        {
            auto uri = gcroot<Uri ^>(_uri);
            auto dict = gcroot<Dictionary<String ^, TestCase ^> ^>(result);
            assembly.EnumerateTestDetails([&](const xUnitpp::TestDetails &td)
                {
                    if (!dict->ContainsKey(marshal_as<String ^>(td.ShortName)))
                    {
                        TestCase ^testCase = gcnew TestCase(marshal_as<String ^>(td.ShortName), uri, marshal_as<String ^>(source));
                        testCase->DisplayName = marshal_as<String ^>(td.ShortName);
                        testCase->CodeFilePath = marshal_as<String ^>(td.LineInfo.file);
                        testCase->LineNumber = td.LineInfo.line;

                        dict->Add(marshal_as<String ^>(td.ShortName), testCase);
                    }
                });
        }

        return result->Values;
    }
}

namespace xUnitpp { namespace VsRunner
{

VsRunner::VsRunner()
    : mUri(gcnew Uri(VSRUNNER_URI))
    , mCancelled(false)
{
}

void VsRunner::DiscoverTests(IEnumerable<String ^> ^sources, IDiscoveryContext ^, IMessageLogger ^, ITestCaseDiscoverySink ^discoverySink)
{
    for each (String ^source in sources)
    {
        for each (TestCase ^test in SingleSourceTestCases(source, mUri))
        {
            discoverySink->SendTestCase(test);
        }
    }
}

void VsRunner::RunTests(IEnumerable<String ^> ^sources, IRunContext ^, IFrameworkHandle ^framework)
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

void VsRunner::RunTests(IEnumerable<TestCase ^> ^tests, IRunContext ^, IFrameworkHandle ^framework)
{
    mCancelled = false;
    RunTests(tests, framework);
}

void VsRunner::Cancel()
{
    mCancelled = true;
}

bool VsRunner::RunTests(IEnumerable<TestCase ^> ^tests, ITestExecutionRecorder ^recorder)
{
    std::map<std::string, std::shared_ptr<TestAssembly>> assemblies;

    for each (TestCase ^test in tests)
    {
        auto source = marshal_as<std::string>(test->Source);
        if (assemblies.find(source) == assemblies.end())
        {
            if (auto assembly = std::make_shared<TestAssembly>(source))
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
