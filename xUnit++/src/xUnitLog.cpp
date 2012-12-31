#include "xUnitLog.h"
#include "EventLevel.h"
#include "TestEventRecorder.h"

namespace xUnitpp
{

Log::Logger::Message::Message(std::function<void(const std::string &, const LineInfo &)> recordMessage, const LineInfo &lineInfo)
    : refCount(*(new size_t(1)))
    , recordMessage(recordMessage)
    , lineInfo(lineInfo)
{
}

Log::Logger::Message::Message(const Message &other)
    : refCount(other.refCount)
    , recordMessage(other.recordMessage)
    , lineInfo(other.lineInfo)
{
    message << other.message.str();
    ++refCount;
}

Log::Logger::Message::~Message()
{
    if (--refCount == 0)
    {
        delete &refCount;
        recordMessage(message.str(), lineInfo);
    }
}

Log::Logger::Logger(std::function<void(const std::string &, const LineInfo &)> recordMessage)
    : recordMessage(recordMessage)
{
}

Log::Logger::Message Log::Logger::operator()(const LineInfo &lineInfo) const
{
    return Message(recordMessage, lineInfo);
}

Log::Log(const TestEventRecorder &recorder)
    : Debug([&](const std::string &msg, const LineInfo &lineInfo) { recorder(TestEvent(EventLevel::Debug, msg, lineInfo)); })
    , Info([&](const std::string &msg, const LineInfo &lineInfo) { recorder(TestEvent(EventLevel::Info, msg, lineInfo)); })
    , Warn([&](const std::string &msg, const LineInfo &lineInfo) { recorder(TestEvent(EventLevel::Warning, msg, lineInfo)); })
{
}

}
