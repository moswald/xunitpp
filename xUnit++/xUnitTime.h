#ifndef XUNITTIME_H_
#define XUNITTIME_H_

#include <chrono>

namespace xUnitpp
{

typedef std::chrono::high_resolution_clock Clock;
typedef decltype(Clock::now()) TimeStamp;

}

#endif
