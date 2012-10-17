#include "LineInfo.h"

namespace xUnitpp
{

LineInfo::LineInfo()
{
}

LineInfo::LineInfo(std::string &&file, int line)
    : file(std::move(file))
    , line(line)
{
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
