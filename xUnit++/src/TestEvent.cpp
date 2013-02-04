#include "TestEvent.h"
#include "EventLevel.h"
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
    if (event.GetIsAssertType())
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
    , message(std::string("Uncaught std::exception: ") + e.what())
{
}

bool TestEvent::GetIsFailure() const
{
    return level > EventLevel::Warning;
}

const char *TestEvent::GetToString() const
{
    if (toString.empty())
    {
        toString = to_string(*this);
    }

    return toString.c_str();
}

const char *TestEvent::GetFile() const
{
    return lineInfo.file.c_str();
}

int TestEvent::GetLine() const
{
    return lineInfo.line;
}

const xUnitpp::ITestAssert &TestEvent::GetAssertInterface() const
{
    return *this;
}

EventLevel TestEvent::GetLevel() const
{
    return level;
}

const char *TestEvent::GetMessage() const
{
    return message.c_str();
}

bool TestEvent::GetIsAssertType() const
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

const char *TestEvent::GetCall() const 
{
    return assert.Call().c_str();
}

const char *TestEvent::GetUserMessage() const 
{
    return assert.UserMessage().c_str();
}

const char *TestEvent::GetCustomMessage() const 
{
    return assert.CustomMessage().c_str();
}

const char *TestEvent::GetExpected() const 
{
    return assert.Expected().c_str();
}

const char *TestEvent::GetActual() const 
{
    return assert.Actual().c_str();
}

}
