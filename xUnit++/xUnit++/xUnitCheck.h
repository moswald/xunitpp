#ifndef XUNITCHECK_H_
#define XUNITCHECK_H_

#include "xUnitAssert.h"

namespace xUnitpp
{

class Check : public Assert
{
    friend class xUnitTest;

public:
    Check();

private:
    const std::vector<xUnitAssert> &Failures() const;

private:
    std::vector<xUnitAssert> failedChecks;
};

}

#endif
