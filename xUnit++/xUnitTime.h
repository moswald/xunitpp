#ifndef XUNITTIME_H_
#define XUNITTIME_H_

#include <chrono>

namespace xUnitpp
{

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds Duration;
typedef std::chrono::duration<float> Seconds;
typedef decltype(Clock::now()) TimeStamp;

inline std::chrono::milliseconds ToMilliseconds(xUnitpp::Duration time)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time);
}

inline xUnitpp::Seconds ToSeconds(xUnitpp::Duration time)
{
    return std::chrono::duration_cast<xUnitpp::Seconds>(time);
}

}

#endif
