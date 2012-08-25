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
            std::cerr << "unable to load " << lib << std::endl;
            continue;
        }

        xUnitpp::ListAllTests listAllTests = (xUnitpp::ListAllTests)GetProcAddress(testlib, "ListAllTests");

        if (!listAllTests)
        {
            std::cerr << "unable to load ListAllTests" << std::endl;
            continue;
        }

        std::vector<xUnitpp::TestDetails> tests;
        listAllTests(tests);

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

        // put together a list of the tests that we will run
        for (const auto &td : tests)
        {
            // check suites:
            // if any suites are specified, a test has to belong to one of them to be run
            if (!options.suites.empty())
            {
                if (std::find(options.suites.begin(), options.suites.end(), td.Suite) == options.suites.end())
                {
                    continue;
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
                    continue;
                }
            }

            // check exclusive attributes:
            // if a test has *all* matching keys, it is excluded
            if (!options.exclusiveAttributes.empty())
            {
                bool excluded = false;

                for (auto test = options.exclusiveAttributes.begin(); !excluded && test != options.exclusiveAttributes.end(); ++test)
                {
                    auto range = td.Attributes.equal_range(test->first);

                    if (range.first != range.second)
                    {
                        if (test->second == "")
                        {
                            excluded = true;
                            break;
                        }

                        for (auto att = range.first; att != range.second; ++att)
                        {
                            if (att->second == test->second)
                            {
                                excluded = true;
                                break;
                            }
                        }
                    }
                }

                if (excluded)
                {
                    continue;
                }
            }

            onList(td);
        }

        if (!options.list)
        {
            std::sort(activeTestIds.begin(), activeTestIds.end());

            xUnitpp::FilteredTestsRunner filteredTestRunner = (xUnitpp::FilteredTestsRunner)GetProcAddress(testlib, "FilteredTestsRunner");

            if (!filteredTestRunner)
            {
                std::cerr << "unable to get RunFilteredTests" << std::endl;
                continue;
            }

            failures += filteredTestRunner(options.timeLimit,
                options.xmlOutput.empty() ? 
                    std::make_shared<xUnitpp::StdOutReporter>(options.verbose, options.veryVerbose) :
                    std::make_shared<xUnitpp::StdOutReporter>(options.verbose, options.veryVerbose),
                [&](const xUnitpp::TestDetails &testDetails)
                {
                    return std::binary_search(activeTestIds.begin(), activeTestIds.end(), testDetails.Id);
                });
        }
    }

    return failures;
}
