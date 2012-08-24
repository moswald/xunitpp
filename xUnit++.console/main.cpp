#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include "CommandLine.h"
#include "StdOutReporter.h"
#include "TestDetails.h"
#include "xUnitTestRunner.h"

int main(int argc, char **argv)
{
    xUnitpp::Utilities::CommandLine::Options options;

    {
        auto result = xUnitpp::Utilities::CommandLine::Parse(argc, argv, options);
        if (!result.empty())
        {
            std::cerr << result;
            return -1;
        }
    }

    int failures = 0;

    for (const auto &lib : options.libraries)
    {
        auto testlib = LoadLibrary(lib.c_str());

        if (testlib == nullptr)
        {
            std::cerr << "unable to load " << argv[1] << std::endl;
            return -1;
        }

        xUnitpp::FilteredTestsRunner filteredTestRunner = (xUnitpp::FilteredTestsRunner)GetProcAddress(testlib, "FilteredTestsRunner");

        if (!filteredTestRunner)
        {
            std::cerr << "unable to get RunFilteredTests" << std::endl;
            return -1;
        }

        failures += filteredTestRunner(options.timeLimit,
            options.xmlOutput.empty() ? 
                std::make_shared<xUnitpp::StdOutReporter>(options.verbose, options.veryVerbose) :
                std::make_shared<xUnitpp::StdOutReporter>(options.verbose, options.veryVerbose),
            [&](const xUnitpp::TestDetails &testDetails)
            {
                return false;
            });
    }

    return failures;
}
