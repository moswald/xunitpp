#include "xUnitCheck.h"
#include "TestEvent.h"

namespace xUnitpp
{

Check::Check()
    : Assert("Check.", [&](const xUnitAssert &assert)
        {
            sinks[std::this_thread::get_id()](TestEvent(assert));
        })
{
}

void Check::SetSink(std::function<void(TestEvent &&)> sink)
{
    sinks[std::this_thread::get_id()] = sink;
}

}
