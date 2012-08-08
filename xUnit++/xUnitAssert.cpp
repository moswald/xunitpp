#include "xUnitAssert.h"

namespace xUnitpp
{

xUnitAssert::xUnitAssert()
{
}

xUnitAssert::xUnitAssert(const std::string &msg)
    : base(msg.c_str())
{
}

}
