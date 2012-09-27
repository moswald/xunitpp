#ifndef ITESTEVENTSOURCE_H_
#define ITESTEVENTSOURCE_H_

#include <functional>

namespace xUnitpp
{

class TestEvent;

struct ITestEventSource
{
    virtual ~ITestEventSource();

    virtual void SetSink(std::function<void(TestEvent &&)> sink) = 0;
};

}

#endif
