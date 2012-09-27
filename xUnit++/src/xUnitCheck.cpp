#include "xUnitCheck.h"
#include "TestEvent.h"
#include "TestEventRecorder.h"

namespace xUnitpp
{

Check::Check(const TestEventRecorder &recorder)
    : Assert("Check.", [&](const xUnitAssert &assert)
        {
            recorder(TestEvent(EventLevel::Check, assert));
        })
{
}

}
