#include "TestEvent.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

const std::string &to_string(EventLevel level)
{
    static std::string msg[] =
    {
        "DEBUG",
        "info",
        "warning",
        "error",
        "error",
        "error"
    };

    return msg[(int)level];
}

const std::string &to_string(const TestEvent &evt)
{
    return evt.message;
}

TestEvent::TestEvent(EventLevel level, const std::string &message)
    : level(level)
    , message(message)
    , lineInfo(LineInfo::empty())
{
}

TestEvent::TestEvent(EventLevel level, const xUnitAssert &assert)
    : level(level)
    , message(assert.what())
    , lineInfo(assert.LineInfo())
{
}

TestEvent::TestEvent(const std::exception &e)
    : level(EventLevel::Fatal)
    , message(e.what())
    , lineInfo(LineInfo::empty())
{
}

bool TestEvent::IsFailure() const
{
    return level > EventLevel::Warning;
}

EventLevel TestEvent::Level() const
{
    return level;
}

const xUnitpp::LineInfo &TestEvent::LineInfo() const
{
    return lineInfo;
}

}
