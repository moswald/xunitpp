#pragma once

#define VSRUNNER_URI "executor://bitbucket.org/moswald/xunit"

namespace xUnitpp { namespace VsRunner
{
    namespace G = System::Collections::Generic;
    namespace OM = Microsoft::VisualStudio::TestPlatform::ObjectModel;
    namespace A = Microsoft::VisualStudio::TestPlatform::ObjectModel::Adapter;
    namespace L = Microsoft::VisualStudio::TestPlatform::ObjectModel::Logging;

    [OM::FileExtension(".dll")]
    [OM::FileExtension(".exe")]
    [OM::DefaultExecutorUri(VSRUNNER_URI)]
    [OM::ExtensionUri(VSRUNNER_URI)]
    public ref class VsRunner : public A::ITestDiscoverer, public A::ITestExecutor
    {
    public:
        VsRunner();

        // ITestDiscoverer
        virtual void DiscoverTests(G::IEnumerable<System::String ^> ^sources, A::IDiscoveryContext ^ctx, L::IMessageLogger ^logger, A::ITestCaseDiscoverySink ^discoverySink);

        // ITestExecutor
        virtual void RunTests(G::IEnumerable<System::String ^> ^sources, A::IRunContext ^ctx, A::IFrameworkHandle ^handle);
        virtual void RunTests(G::IEnumerable<OM::TestCase ^> ^tests, A::IRunContext ^ctx, A::IFrameworkHandle ^handle);
        virtual void Cancel();

    private:
        bool RunTests(G::IEnumerable<OM::TestCase ^> ^tests, L::IMessageLogger ^logger);

    private:
        System::Uri ^mUri;
        bool mCancelled;
    };
}}
