#include "LineInfo.h"

namespace xUnitpp
{

LineInfo::LineInfo()
{
}

LineInfo::LineInfo(const std::string &file, int line)
    : file(file)
    , line(line)
{
}

const LineInfo &LineInfo::empty()
{
    static LineInfo empty;
    return empty;
}

std::string to_string(const LineInfo &lineInfo)
{
    if (lineInfo.file.empty())
    {
        return std::string();
    }

    return lineInfo.file + "(" + std::to_string(lineInfo.line) + ")";
}

}
