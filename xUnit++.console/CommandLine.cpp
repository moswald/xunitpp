#include "CommandLine.h"
#include <filesystem>
#include <functional>
#include <stack>
#include <sstream>

namespace
{
    std::string PopTop(std::stack<std::string> &stack)
    {
        auto s = stack.top();
        stack.pop();
        return s;
    }

    std::string EatKeyValuePairs(const std::string &opt, std::stack<std::string> &arguments, std::function<void (std::tuple<std::string, std::string> &&)> onParsedPair)
    {
        if (arguments.empty())
        {
            return opt + " expects at least one attribute argument.";
        }

        do
        {
            auto raw = PopTop(arguments);

            auto pos = raw.find('=');
            if (pos == std::string::npos)
            {
                return raw + " is not a valid format for " + opt + " (should be \"name=[value]\").";
            }

            auto key = raw.substr(0, pos);
            auto value = raw.substr(pos + 1, std::string::npos);

            onParsedPair(std::make_tuple(key, value));
        } while (!arguments.empty() && arguments.top().find('=') != std::string::npos);

        return "";
    }

    bool Throws(std::function<void()> fn)
    {
        try
        {
            fn();
            return false;
        }
        catch (...)
        {
            return true;
        }
    }
}

namespace xUnitpp { namespace Utilities {

namespace CommandLine
{
    Options::Options()
        : verbose(false)
        , timeLimit(0)
    {
    }

    std::string Parse(int argc, char **argv, Options &options)
    {
        auto exe = [=]()
        {
            return std::tr2::sys::path(argv[0]).leaf();
        };

        std::stack<std::string> arguments;

        for (int i = 1; i != argc; ++i)
        {
            arguments.emplace(argv[i]);
        }

        while (!arguments.empty())
        {
            auto opt = PopTop(arguments);

            if (opt[0] == '-')
            {
                if (opt == "-v")
                {
                    options.verbose = true;
                }
                else if (opt == "-i" || opt == "--include")
                {
                    auto error = EatKeyValuePairs(opt, arguments, [&](std::tuple<std::string, std::string> &&kv) { options.inclusiveAttributes.emplace_back(kv); });

                    if (!error.empty())
                    {
                        return error + Usage(exe());
                    }
                }
                else if (opt == "-e" || opt == "--exclude")
                {
                    auto error = EatKeyValuePairs(opt, arguments, [&](std::tuple<std::string, std::string> &&kv) { options.exclusiveAttributes.emplace_back(kv); });

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

                    options.xmlOutput = PopTop(arguments);
                }
                else if (opt == "-t" || opt == "--timelimit")
                {
                    if (arguments.empty() || Throws([&]() { std::istringstream(PopTop(arguments)) >> options.timeLimit; }))
                    {
                        return opt + " expects a following timelimit specified in milliseconds." + Usage(exe());
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
            "  -v                             : Verbose mode: include successful test details\n"
            "  -i --include <NAME=[VALUE]>+   : Include tests with a matching <name=value> attribute\n"
            "  -e --exclude <NAME=[VALUE]>+   : Exclude tests with a matching <name=value> attribute\n"
            "  -t --timelimit <MILLISECONDS>  : Set the default test time limit\n"
            "  -x --xml <FILENAME>            : Output Xunit-style XML file\n"
            "\n"
            "Tests are selected with an OR operation for inclusive attributes.\n"
            "Tests are excluded with an AND operation for exclusive attributes.\n"
            "When VALUE is omitted, any attribute with name NAME is matched.\n";

        return "\nusage: " + exe + usage;
    }
}

}}
