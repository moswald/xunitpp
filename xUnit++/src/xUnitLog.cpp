#include "xUnitLog.h"
#include "TestEventRecorder.h"

namespace xUnitpp
{

Log::Logger::Message::Message(std::function<void(const std::string &)> recordMessage)
    : refCount(*(new size_t(1)))
    , recordMessage(recordMessage)
{
}

Log::Logger::Message::Message(const Message &other)
    : refCount(other.refCount)
    , recordMessage(std::move(other.recordMessage))
    , message(other.message.str())
{
    ++refCount;
}

Log::Logger::Message::~Message()
{
    if (--refCount == 0)
    {
        delete &refCount;
        recordMessage(message.str());
    }
}

Log::Logger::Logger(std::function<void(const std::string &)> recordMessage)
    : recordMessage(recordMessage)
{
}

Log::Log(const TestEventRecorder &recorder)
    : Debug([&](const std::string &msg) { recorder(TestEvent(EventLevel::Debug, msg)); })
    , Info([&](const std::string &msg) { recorder(TestEvent(EventLevel::Info, msg)); })
    , Warn([&](const std::string &msg) { recorder(TestEvent(EventLevel::Warning, msg)); })
{
}

}
