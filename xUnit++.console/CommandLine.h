#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <map>
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
        bool veryVerbose;
        bool list;
        std::vector<std::string> suites;
        std::multimap<std::string, std::string> inclusiveAttributes;
        std::multimap<std::string, std::string> exclusiveAttributes;
        std::vector<std::string> libraries;
        std::string xmlOutput;
        int timeLimit;
        int threadLimit;
    };

    std::string Parse(int argc, char **argv, Options &options);
    std::string Usage(const std::string &exe);
}

}}

#endif
