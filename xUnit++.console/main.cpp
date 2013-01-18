#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <tuple>
#include <vector>
#include "xUnit++/ExportApi.h"
#include "xUnit++/ITestDetails.h"
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
        auto testAssembly = xUnitpp::Utilities::TestAssembly(lib.c_str(), options.shadowCopy);

        if (!testAssembly)
        {
            std::cerr << "Unable to load " << lib << std::endl;
            forcedFailure = true;
            continue;
        }

        std::vector<int> activeTestIds;
        auto onList = [&](const xUnitpp::ITestDetails &td)
            {
                if (options.list)
                {
                    std::cout << std::endl;
                    for (auto i = 0U; i != td.GetAttributeCount(); ++i)
                    {
                        std::cout << (std::string("[") + td.GetAttributeKey(i) + " = " + td.GetAttributeValue(i) + "]") << std::endl;
                    }

                    std::cout << (td.GetSuite() + std::string(" :: ") + td.GetName()) << std::endl;
                }
                else
                {
                    activeTestIds.push_back(td.GetId());
                }
            };

        testAssembly.EnumerateTestDetails([&](const xUnitpp::ITestDetails &td)
            {
                // check suites:
                // if any suites are specified, a test has to belong to one of them to be run
                if (!options.suites.empty())
                {
                    bool included = false;
                    for (const auto &suite : options.suites)
                    {
                        std::regex regex(suite, std::regex_constants::icase);

                        std::string testSuite = td.GetSuite() == nullptr ? "" : td.GetSuite();
                        if (std::regex_search(testSuite, regex))
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
                        std::regex regex(name, std::regex_constants::icase);

                        if (std::regex_search(td.GetName(), regex))
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
                        size_t begin, end;
                        td.FindAttributeKey(test->first.c_str(), begin, end);

                        if (begin != end)
                        {
                            if (test->second == "")
                            {
                                included = true;
                                break;
                            }

                            for (auto it = begin; it != end; ++it)
                            {
                                if (td.GetAttributeValue(it) == test->second)
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
                        size_t begin, end;
                        td.FindAttributeKey(test->first.c_str(), begin, end);

                        if (begin != end)
                        {
                            // key matched, and we want to exclude all tests with a specific value
                            if (test->second != "")
                            {
                                for (; begin != end; ++begin)
                                {
                                    if (td.GetAttributeValue(begin) == test->second)
                                    {
                                        break;
                                    }
                                }

                                if (begin == end)
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

            auto runTests = [&](xUnitpp::IOutput &reporter)
                {
                    totalFailures += testAssembly.FilteredTestsRunner(options.timeLimit, options.threadLimit, reporter,
                        [&](const xUnitpp::ITestDetails &testDetails)
                        {
                            return std::binary_search(activeTestIds.begin(), activeTestIds.end(), testDetails.GetId());
                        });
                };

            if (options.xmlOutput.empty())
            {
                xUnitpp::ConsoleReporter reporter(options.verbose, options.sort, options.group);
                runTests(reporter);
            }
            else if (options.xmlOutput == ".")
            {
                xUnitpp::Utilities::XmlReporter reporter(std::cout);
                runTests(reporter);
            }
            else
            {
                std::ofstream file(options.xmlOutput, std::ios::binary);

                if (!file)
                {
                    std::cerr << "Unable to open " << options.xmlOutput << " for writing.\n\n";
                }

                xUnitpp::Utilities::XmlReporter reporter(!file ? std::cerr : file);
                runTests(reporter);
            }
        }
    }

    return forcedFailure ? 1 : -totalFailures;
}
