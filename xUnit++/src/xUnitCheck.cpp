#include "xUnitCheck.h"

namespace xUnitpp
{

Check::Check()
    : Assert("Check.",
        [&](xUnitAssert assert)
        {
            return xUnitFailure(assert,
                [&](const xUnitAssert &assert)
                {
                    failedChecks.emplace_back(assert);
                });
        })
{
}

const std::vector<xUnitAssert> &Check::Failures() const
{
    return failedChecks;
}

}
