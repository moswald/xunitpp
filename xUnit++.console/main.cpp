#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include "xUnit++/ExportApi.h"
#include "xUnit++/TestDetails.h"
#include "CommandLine.h"
#include "ConsoleReporter.h"
#include "TestAssembly.h"
#include "XmlReporter.h"

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

    int totalFailures = 0;
    bool forcedFailure = false;

    for (const auto &lib : options.libraries)
    {
        auto testAssembly = xUnitpp::Utilities::TestAssembly(lib.c_str());

        if (!testAssembly)
        {
            std::cerr << "Unable to load " << lib << std::endl;
            forcedFailure = true;
            continue;
        }

        std::vector<int> activeTestIds;
        auto onList = [&](const xUnitpp::TestDetails &td)
            {
                if (options.list)
                {
                    std::cout << std::endl;
                    for (const auto &att : td.Attributes)
                    {
                        std::cout << ("[" + att.first + " = " + att.second + "]") << std::endl;
                    }

                    std::cout << (td.Suite + " :: " + td.Name) << std::endl;
                }
                else
                {
                    activeTestIds.push_back(td.Id);
                }
            };

        testAssembly.EnumerateTestDetails([&](const xUnitpp::TestDetails &td)
            {
                // check suites:
                // if any suites are specified, a test has to belong to one of them to be run
                if (!options.suites.empty())
                {
                    bool included = false;
                    for (const auto &suite : options.suites)
                    {
                        if (td.Suite.find(suite) != std::string::npos)
                        {
                            included = true;
                            break;
                        }
                    }

                    if (!included)
                    {
                        return;
                    }
                }

                // check names
                if (!options.testNames.empty())
                {
                    bool included = false;
                    for (const auto &name : options.testNames)
                    {
                        if (td.ShortName.find(name) != std::string::npos)
                        {
                            included = true;
                            break;
                        }
                    }

                    if (!included)
                    {
                        return;
                    }
                }

                // check inclusive attributes:
                // a test has to have *any* matching attribute key and value to be run
                if (!options.inclusiveAttributes.empty())
                {
                    bool included = false;

                    for (auto test = options.inclusiveAttributes.begin(); !included && test != options.inclusiveAttributes.end(); ++test)
                    {
                        auto range = td.Attributes.find(*test);

                        if (range.first != range.second)
                        {
                            if (test->second == "")
                            {
                                included = true;
                                break;
                            }

                            for (auto it = range.first; it != range.second; ++it)
                            {
                                if (range.first->second == test->second)
                                {
                                    included = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!included)
                    {
                        return;
                    }
                }

                // check exclusive attributes:
                // if a test has *all* matching keys, it is excluded
                if (!options.exclusiveAttributes.empty())
                {
                    bool matchFailed = false;
                    for (auto test = options.exclusiveAttributes.begin(); !matchFailed && test != options.exclusiveAttributes.end(); ++test)
                    {
                        auto range = td.Attributes.find(*test);

                        if (range.first != range.second)
                        {
                            // key matched, and we want to exclude all tests with a specific value
                            if (test->second != "")
                            {
                                auto it = range.first;
                                for (; it != range.second; ++it)
                                {
                                    if (it->second == test->second)
                                    {
                                        break;
                                    }
                                }

                                if (it == range.second)
                                {
                                    matchFailed = true;
                                }
                            }
                        }
                    }

                    if (!matchFailed)
                    {
                        return;
                    }
                }

                onList(td);
            });

        if (!activeTestIds.empty())
        {
            std::sort(activeTestIds.begin(), activeTestIds.end());

            std::unique_ptr<xUnitpp::IOutput> reporter(options.xmlOutput.empty() ?
                (xUnitpp::IOutput *)new xUnitpp::ConsoleReporter(options.verbose, options.veryVerbose) :
                (xUnitpp::IOutput *)new xUnitpp::Utilities::XmlReporter(options.xmlOutput));
            totalFailures += testAssembly.FilteredTestsRunner(options.timeLimit, options.threadLimit, *reporter,
                [&](const xUnitpp::TestDetails &testDetails)
                {
                    return std::binary_search(activeTestIds.begin(), activeTestIds.end(), testDetails.Id);
                });
        }
    }

    return forcedFailure ? 1 : -totalFailures;
}
