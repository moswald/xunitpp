#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include "CommandLine.h"
#include "ExportApi.h"
#include "StdOutReporter.h"
#include "TestDetails.h"
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
        auto testlib = LoadLibrary(lib.c_str());

        if (testlib == nullptr)
        {
            std::cerr << "unable to load " << lib << std::endl;
            forcedFailure = true;
            continue;
        }

        xUnitpp::EnumerateTestDetails enumerateTests = (xUnitpp::EnumerateTestDetails)GetProcAddress(testlib, "EnumerateTestDetails");

        if (!enumerateTests)
        {
            std::cerr << "unable to load EnumerateTestDetails" << std::endl;
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

        enumerateTests([&](const xUnitpp::TestDetails &td)
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

                    for (auto att = td.Attributes.begin(); !included && att != td.Attributes.end(); ++att)
                    {
                        auto it = options.inclusiveAttributes.equal_range(att->first);

                        for (auto test = it.first; test != it.second; ++test)
                        {
                            if (test->second == "" || test->second == att->second)
                            {
                                included = true;
                                break;
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
                    for (auto test = options.exclusiveAttributes.begin(); test != options.exclusiveAttributes.end(); ++test)
                    {
                        auto range = td.Attributes.equal_range(test->first);

                        if (range.first != range.second)
                        {
                            if (test->second == "")
                            {
                                return;
                            }

                            for (auto att = range.first; att != range.second; ++att)
                            {
                                if (att->second == test->second)
                                {
                                    return;
                                }
                            }
                        }
                    }
                }

                onList(td);
            });

        if (!activeTestIds.empty())
        {
            std::sort(activeTestIds.begin(), activeTestIds.end());

            xUnitpp::FilteredTestsRunner filteredTestRunner = (xUnitpp::FilteredTestsRunner)GetProcAddress(testlib, "FilteredTestsRunner");

            if (!filteredTestRunner)
            {
                std::cerr << "unable to get FilteredTestsRunner" << std::endl;
                forcedFailure = true;
                continue;
            }

            std::unique_ptr<xUnitpp::IOutput> reporter(options.xmlOutput.empty() ?
                (xUnitpp::IOutput *)new xUnitpp::StdOutReporter(options.verbose, options.veryVerbose) :
                (xUnitpp::IOutput *)new xUnitpp::XmlReporter(options.xmlOutput));
            totalFailures += filteredTestRunner(options.timeLimit, options.threadLimit, *reporter,
                [&](const xUnitpp::TestDetails &testDetails)
                {
                    return std::binary_search(activeTestIds.begin(), activeTestIds.end(), testDetails.Id);
                });
        }
    }

    return forcedFailure ? 1 : totalFailures;
}
