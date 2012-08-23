#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

#include <string>
#include <tuple>
#include <vector>

namespace xUnitpp
{

namespace Utilities
{
    class CommandLine
    {
    public:
        CommandLine(int argc, char **argv);

        static std::string Usage(const std::string &exe);

        const std::vector<std::string> &TestLibraries();

    private:
        bool mVerbose;
        std::vector<std::tuple<std::string, std::string>> mInclusiveAttributes;
        std::vector<std::tuple<std::string, std::string>> mExclusiveAttributes;
        std::vector<std::string> mTestLibraries;
        std::string mXmlOutput;
        int mTimeLimit;
    };
}

}

#endif
