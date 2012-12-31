#include "xUnitWarn.h"
#include "EventLevel.h"
#include "TestEvent.h"
#include "TestEventRecorder.h"

namespace xUnitpp
{

Warn::Warn(const TestEventRecorder &recorder)
    : Assert("Warn.", [&](const xUnitAssert &assert)
        {
            recorder(TestEvent(EventLevel::Warning, assert));
        })
{
}

}