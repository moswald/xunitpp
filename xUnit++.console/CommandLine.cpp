#include "CommandLine.h"
#include <algorithm>
#include <cctype>
#include <functional>
#include <queue>
#include <sstream>

namespace
{
    std::string TakeFront(std::queue<std::string> &queue)
    {
        auto s = queue.front();
        queue.pop();
        return s;
    }

    std::string EatKeyValuePairs(const std::string &opt, std::queue<std::string> &arguments, std::function<void (std::pair<std::string, std::string> &&)> onParsedPair)
    {
        if (arguments.empty())
        {
            return opt + " expects at least one attribute argument.";
        }

        do
        {
            auto raw = TakeFront(arguments);

            auto pos = raw.find('=');
            if (pos == std::string::npos)
            {
                return raw + " is not a valid format for " + opt + " (should be \"name=[value]\").";
            }

            auto key = raw.substr(0, pos);
            auto value = raw.substr(pos + 1, std::string::npos);

            onParsedPair(std::make_pair(key, value));
        } while (!arguments.empty() && arguments.front().find('=') != std::string::npos);

        return "";
    }

    bool GetInt(std::queue<std::string> &queue, int &value)
    {
        std::istringstream stream(TakeFront(queue));

        stream >> value;

        return stream.failbit;
    }
}

namespace xUnitpp { namespace Utilities {

namespace CommandLine
{
    Options::Options()
        : verbose(false)
        , list(false)
        , timeLimit(0)
        , threadLimit(0)
        , shadowCopy(true)
        , sort(false)
        , group(false)
    {
    }

    std::string Parse(int argc, char **argv, Options &options)
    {
        auto exe = [=]() -> std::string
        {
            std::string path = argv[0];
            std::replace(path.begin(), path.end(), '\\', '/');

            auto idx = path.find_last_of('/');

            if (idx != std::string::npos)
            {
                return path.substr(idx + 1);
            }

            return path;
        };

        std::queue<std::string> arguments;

        for (int i = 1; i != argc; ++i)
        {
            arguments.emplace(argv[i]);
        }

        while (!arguments.empty())
        {
            auto opt = TakeFront(arguments);

            if (opt[0] == '-')
            {
                if (opt == "-v" || opt == "--verbose")
                {
                    options.verbose = true;
                }
                else if (opt == "-l" || opt == "--list")
                {
                    options.list = true;
                }
                else if (opt == "-s" || opt == "--suite")
                {
                    if (arguments.empty())
                    {
                        return opt + " expects a following suite name argument." + Usage(exe());
                    }

                    options.suites.push_back(TakeFront(arguments));
                }
                else if (opt == "-n" || opt == "--name")
                {
                    if (arguments.empty())
                    {
                        return opt + " expects a following test name argument." + Usage(exe());
                    }

                    options.testNames.push_back(TakeFront(arguments));
                }
                else if (opt == "-i" || opt == "--include")
                {
                    auto error = EatKeyValuePairs(opt, arguments, [&](std::pair<std::string, std::string> &&kv) { options.inclusiveAttributes.insert(kv); });

                    if (!error.empty())
                    {
                        return error + Usage(exe());
                    }
                }
                else if (opt == "-e" || opt == "--exclude")
                {
                    auto error = EatKeyValuePairs(opt, arguments, [&](std::pair<std::string, std::string> &&kv) { options.exclusiveAttributes.insert(kv); });

                    if (!error.empty())
                    {
                        return error + Usage(exe());
                    }
                }
                else if (opt == "-x" || opt == "--xml")
                {
                    if (arguments.empty() || arguments.front().front() == '-')
                    {
                        // "." is a special filename meaning "use stdout"
                        options.xmlOutput = ".";
                    }
                    else
                    {
                        options.xmlOutput = TakeFront(arguments);
                    }
                }
                else if (opt == "-t" || opt == "--timelimit")
                {
                    if (arguments.empty() || !GetInt(arguments, options.timeLimit))
                    {
                        return opt + " expects a following timelimit specified in milliseconds." + Usage(exe());
                    }
                }
                else if (opt == "-c" || opt == "--concurrent")
                {
                    if (arguments.empty() || !GetInt(arguments, options.threadLimit))
                    {
                        return opt + " expects a following test limit count." + Usage(exe());
                    }
                }
                else if (opt == "-o" || opt == "--sort")
                {
                    options.sort = true;
                }
                else if (opt == "-g" || opt == "--group")
                {
                    options.sort = true;
                    options.group = true;
                }
                else if (opt == "--no-shadow")
                {
                    options.shadowCopy = false;
                }
                else
                {
                    return "Unrecognized option " + opt + "." + Usage(exe());
                }
            }
            else
            {
                //for (auto &c : opt)
                //{
                //    c = (char)std::tolower(c);
                //}

                options.libraries.insert(opt);
            }
        }

        if (options.libraries.empty())
        {
            return "At least one testLibrary must be specified." + Usage(exe());
        }

        return "";
    }

    std::string Usage(const std::string &exe)
    {
        static const std::string usage =
            " <testLibrary>+ [option]+\n"
            "\n"
            "options:\n\n"
            "  -v --verbose                   : Verbose mode: include successful test timing\n"
            "  -l --list                      : Do not run tests, just list the ones that pass the filters\n"
            "  -s --suite <SUITE>+            : Suite(s) of tests to run (regex match)\n"
            "  -n --name <TEST>+              : Test(s) to run (regex match)\n"
            "  -i --include <NAME=[VALUE]>+   : Include tests with exactly matching <name=value> attribute(s)\n"
            "  -e --exclude <NAME=[VALUE]>+   : Exclude tests with exactly matching <name=value> attribute(s)\n"
            "  -t --timelimit <milliseconds>  : Set the default test time limit\n"
            "  -x --xml [FILENAME]            : Output Xunit-style XML, to optional file named FILENAME\n"
            "  -c --concurrent <max tests>    : Set maximum number of concurrent tests\n"
            "  -o --sort                      : Sort tests by suite and then by test name\n"
            "  -g --group                     : Group test output under suite headers (implies --sort)\n"
            "     --no-shadow                 : Disable shadow copying the test binaries\n"
            "\n"
            "Tests are selected with an OR operation for inclusive attributes.\n"
            "Tests are excluded with an AND operation for exclusive attributes.\n"
            "When VALUE is omitted, any attribute with name NAME is matched.\n"
            "\n"
            "Sorting and grouping test output causes test results to be cached until after all tests have completed.\n"
            "Normally, test results are printed as soon as the test is complete.\n";

        return "\nusage: " + exe + usage;
    }
}

}}
