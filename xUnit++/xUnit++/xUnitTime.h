#ifndef XUNITTIME_H_
#define XUNITTIME_H_

#include <chrono>

namespace xUnitpp { namespace Time
{

typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::nanoseconds Duration;
typedef std::chrono::duration<float> Seconds;
typedef decltype(Clock::now()) TimeStamp;

inline std::chrono::milliseconds ToMilliseconds(Duration time)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(time);
}

inline Seconds ToSeconds(Duration time)
{
    return std::chrono::duration_cast<Seconds>(time);
}

template<typename TDuration>
inline Duration ToDuration(TDuration time)
{
    return std::chrono::duration_cast<Duration>(time);
}

}}

#endif
