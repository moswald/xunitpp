#ifndef LINEINFO_H_
#define LINEINFO_H_

#include <string>

namespace xUnitpp
{

struct LineInfo
{
    LineInfo(const std::string &file, int line);

    static const LineInfo &empty();

    std::string file;
    int line;

private:
    LineInfo();
};

}

#endif
