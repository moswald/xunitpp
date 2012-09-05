#include "CommandLine.h"
#include <filesystem>
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
        , veryVerbose(false)
        , list(false)
        , timeLimit(0)
        , threadLimit(0)
    {
    }

    std::string Parse(int argc, char **argv, Options &options)
    {
        auto exe = [=]()
        {
            return std::tr2::sys::path(argv[0]).leaf();
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
                if (opt == "-v")
                {
                    options.verbose = true;
                }
                else if (opt == "-vv")
                {
                    options.verbose = true;
                    options.veryVerbose = true;
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
                    auto error = EatKeyValuePairs(opt, arguments, [&](std::pair<std::string, std::string> &&kv) { options.inclusiveAttributes.emplace(kv); });

                    if (!error.empty())
                    {
                        return error + Usage(exe());
                    }
                }
                else if (opt == "-e" || opt == "--exclude")
                {
                    auto error = EatKeyValuePairs(opt, arguments, [&](std::pair<std::string, std::string> &&kv) { options.exclusiveAttributes.emplace(kv); });

                    if (!error.empty())
                    {
                        return error + Usage(exe());
                    }
                }
                else if (opt == "-x" || opt == "--xml")
                {
                    if (arguments.empty())
                    {
                        return opt + " expects a following filename argument." + Usage(exe());
                    }

                    options.xmlOutput = TakeFront(arguments);
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
                else
                {
                    return "Unrecognized option " + opt + "." + Usage(exe());
                }
            }
            else
            {
                options.libraries.push_back(opt);
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
            "  -v                             : Verbose mode: include successful test timing\n"
            "  -vv                            : Very verbose: write test start message\n"
            "  -l --list                      : Do not run tests, just list the ones that pass the filters\n"
            "  -s --suite <SUITE>+            : Suite(s) of tests to run (substring match)\n"
            "  -n --name <TEST>+              : Test(s) to run (substring match)\n"
            "  -i --include <NAME=[VALUE]>+   : Include tests with matching <name=value> attribute(s)\n"
            "  -e --exclude <NAME=[VALUE]>+   : Exclude tests with matching <name=value> attribute(s)\n"
            "  -t --timelimit <milliseconds>  : Set the default test time limit\n"
            "  -x --xml <FILENAME>            : Output Xunit-style XML file\n"
            "  -c --concurrent <max ttests>   : Set maximum number of concurrent tests\n"
            "\n"
            "Tests are selected with an OR operation for inclusive attributes.\n"
            "Tests are excluded with an AND operation for exclusive attributes.\n"
            "When VALUE is omitted, any attribute with name NAME is matched.\n";

        return "\nusage: " + exe + usage;
    }
}

}}
