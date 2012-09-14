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
    Check(const Check &) /* = delete */;
    Check(Check &&) /* = delete */;
    Check &operator =(Check) /* = delete */;

    const std::vector<xUnitAssert> &Failures() const;

private:
    std::vector<xUnitAssert> failedChecks;
};

}

#endif
