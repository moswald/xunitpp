#include "TestEventRecorder.h"
#include "TestEvent.h"

namespace xUnitpp
{

void TestEventRecorder::Tie(std::function<void(TestEvent &&)> sink)
{
    std::lock_guard<std::mutex> guard(lock);

    // replace, don't insert: threads may be reused for future tests
    sinks[std::this_thread::get_id()] = sink;
}

void TestEventRecorder::operator()(TestEvent &&evt) const
{
    std::lock_guard<std::mutex> guard(lock);
    sinks[std::this_thread::get_id()](std::move(evt));
}

}
