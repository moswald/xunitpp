#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <string>
#include <tuple>
#include <vector>

namespace xUnitpp { namespace Utilities {

namespace CommandLine
{
    struct Options
    {
        Options();

        bool verbose;
        std::vector<std::tuple<std::string, std::string>> inclusiveAttributes;
        std::vector<std::tuple<std::string, std::string>> exclusiveAttributes;
        std::vector<std::string> libraries;
        std::string xmlOutput;
        int timeLimit;
    };

    std::string Parse(int argc, char **argv, Options &options);
    std::string Usage(const std::string &exe);
}

}}

#endif
