#ifndef SUITE_H_
#define SUITE_H_

#include <string>

namespace xUnitSuite
{
    inline const std::string &Name()
    {
        static std::string name = "xUnit++ Default Suite";
        return name;
    }

}

#endif
