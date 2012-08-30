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

}
