#ifndef XUNITASSERT_H_
#define XUNITASSERT_H_

#include <exception>
#include <string>

namespace xUnitpp
{

class xUnitAssert : public std::exception
{
    typedef std::exception base;

public:
    xUnitAssert();
    explicit xUnitAssert(const std::string &msg);
};

}

#endif
