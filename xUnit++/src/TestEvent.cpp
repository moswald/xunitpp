#include "TestEvent.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

const std::string &to_string(EventLevel level)
{
    static std::string msg[] =
    {
        "[   DEBUG ]",
        "[    Info ]",      
        "[ Warning ]",
        "[   Check ]",
        "[  Assert ]",
        "[   Fatal ]"
    };

    return msg[(int)level];
}

std::string to_string(const TestEvent &event)
{
    if (event.IsAssertType())
    {
        auto &&assert = event.Assert();
        auto message = assert.Call() + "() failure";

        std::string userMessage = assert.UserMessage();
        if (!userMessage.empty())
        {
            message += ": " + userMessage;

            if (!assert.CustomMessage().empty())
            {
                message += "\n     " + assert.CustomMessage();
            }
        }
        else if (!assert.CustomMessage().empty())
        {
            message += ": " + assert.CustomMessage();
        }
        else
        {
            message += ".";
        }

        if (!assert.Expected().empty() || !assert.Actual().empty())
        {
            message += "\n     Expected: ";
            message += assert.Expected();
            message += "\n       Actual: ";
            message += assert.Actual();
        }

        return message;
    }

    return event.message;
}

TestEvent::TestEvent(EventLevel level, const std::string &message, const xUnitpp::LineInfo &lineInfo)
    : level(level)
    , assert(xUnitAssert::None())
    , message(message)
    , lineInfo(lineInfo)
{
}

TestEvent::TestEvent(EventLevel level, const xUnitAssert &assert)
    : level(level)
    , assert(assert)
    , lineInfo(assert.LineInfo())
{
}

TestEvent::TestEvent(const std::exception &e)
    : level(EventLevel::Fatal)
    , assert(xUnitAssert::None())
    , message(e.what())
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

bool TestEvent::IsAssertType() const
{
    return !assert.Call().empty();
}

const xUnitpp::xUnitAssert &TestEvent::Assert() const
{
    return assert;
}

const std::string &TestEvent::Message() const
{
    return message;
}

const xUnitpp::LineInfo &TestEvent::LineInfo() const
{
    return lineInfo;
}

}
