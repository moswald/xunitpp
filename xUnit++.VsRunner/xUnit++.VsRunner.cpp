#include "stdafx.h"
#include "xUnit++.VsRunner.h"
#include <algorithm>
#include <map>
#include <memory>
#include <vector>
#include <msclr/marshal_cppstd.h>
#include "ExportApi.h"
#include "IOutput.h"
#include "LineInfo.h"
#include "TestDetails.h"

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
                this->testCases.Add(test->FullyQualifiedName, test);
            }
        }

        void ReportStart(const xUnitpp::TestDetails &td, int)
        {
            auto name = marshal_as<String ^>(td.Name);
            recorder->RecordStart(testCases[name]);

            auto result = gcnew TestResult(testCases[name]);
            result->ComputerName = Environment::MachineName;
            result->DisplayName = name;
            result->Outcome = TestOutcome::None;

            testResults.Add(name, result);
        }

        void ReportFailure(const xUnitpp::TestDetails &td, int, const std::string &msg, const xUnitpp::LineInfo &)
        {
            auto result = testResults[marshal_as<String ^>(td.Name)];
            result->Outcome = TestOutcome::Failed;
            result->ErrorMessage = marshal_as<String ^>(msg);
        }

        void ReportSkip(const xUnitpp::TestDetails &td, const std::string &)
        {
            auto testCase = testCases[marshal_as<String ^>(td.Name)];
            auto result = gcnew TestResult(testCase);
            result->ComputerName = Environment::MachineName;
            result->DisplayName = marshal_as<String ^>(td.Name);
            result->Duration = TimeSpan::FromSeconds(0);
            result->Outcome = TestOutcome::Skipped;
            recorder->RecordEnd(testCase, result->Outcome);
            recorder->RecordResult(result);
        }

        void ReportFinish(const xUnitpp::TestDetails &td, int, xUnitpp::Duration timeTaken)
        {
            auto result = testResults[marshal_as<String ^>(td.Name)];
            result->Duration = TimeSpan::FromSeconds(xUnitpp::ToSeconds(timeTaken).count());

            if (result->Outcome == TestOutcome::None)
            {
                result->Outcome = TestOutcome::Passed;
            }

            recorder->RecordEnd(testCases[marshal_as<String ^>(td.Name)], result->Outcome);
            recorder->RecordResult(result);
        }

        void ReportAllTestsComplete(size_t, size_t, size_t, xUnitpp::Duration)
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

        virtual void ReportStart(const xUnitpp::TestDetails &td, int dataIndex) override
        {
            reporter->ReportStart(td, dataIndex);
        }

        virtual void ReportFailure(const xUnitpp::TestDetails &testDetails, int dataIndex, const std::string &msg, const xUnitpp::LineInfo &lineInfo) override
        {
            reporter->ReportFailure(testDetails, dataIndex, msg, lineInfo);
        }

        virtual void ReportSkip(const xUnitpp::TestDetails &testDetails, const std::string &reason) override
        {
            reporter->ReportSkip(testDetails, reason);
        }

        virtual void ReportFinish(const xUnitpp::TestDetails &testDetails, int dataIndex, xUnitpp::Duration timeTaken) override
        {
            reporter->ReportFinish(testDetails, dataIndex, timeTaken);
        }

        virtual void ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, xUnitpp::Duration totalTime) override
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
            FilteredTestsRunner(0, reporter,
                [&](const xUnitpp::TestDetails &testDetails)
                {
                    return !cancelled && std::find_if(tests.begin(), tests.end(),
                        [&](gcroot<TestCase ^> test)
                        {
                            return marshal_as<std::string>(test->DisplayName) == testDetails.Name;
                        }) != tests.end();
                });
        }

    private:
        xUnitpp::FilteredTestsRunner FilteredTestsRunner;
        std::string tempFile;
        HMODULE module;
        std::vector<gcroot<TestCase ^>> tests;
    };

    IEnumerable<TestCase ^> ^SingleSourceTestCases(String ^source, Uri ^_uri)
    {
        auto result = gcnew List<TestCase ^>();

        auto name = marshal_as<std::string>(source);
        if (auto assembly = TestAssembly(name))
        {
            auto uri = gcroot<Uri ^>(_uri);
            auto list = gcroot<List<TestCase ^> ^>(result);
            assembly.EnumerateTestDetails([&](const xUnitpp::TestDetails &td)
                {
                    TestCase ^testCase = gcnew TestCase(marshal_as<String ^>(td.Name.c_str()), uri, marshal_as<String ^>(name));
                    testCase->CodeFilePath = marshal_as<String ^>(td.Filename);
                    testCase->DisplayName = marshal_as<String ^>(td.Name);
                    testCase->LineNumber = td.Line;

                    list->Add(testCase);
                });
        }

        return result;
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
        if (!RunTests(SingleSourceTestCases(source, mUri), framework))
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
