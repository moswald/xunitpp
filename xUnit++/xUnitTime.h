#ifndef XUNITTIME_H_
#define XUNITTIME_H_

#include <chrono>

namespace xUnitpp
{

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::duration<float, std::milli> milliseconds;

}

#endif
