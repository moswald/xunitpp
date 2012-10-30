#ifndef TESTEVENT_H_
#define TESTEVENT_H_

#include <exception>
#include <string>
#include "LineInfo.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

enum class EventLevel
{
    Debug,
    Info,
    Warning,
    Check,
    Assert,
    Fatal
};

const std::string &to_string(EventLevel level);

class TestEvent
{
public:
    TestEvent(EventLevel level, const std::string &message, const LineInfo &lineInfo = xUnitpp::LineInfo());
    TestEvent(EventLevel level, const xUnitAssert &assert);
    TestEvent(const std::exception &e);

    bool IsFailure() const;
    bool IsAssertType() const;

    const xUnitAssert &Assert() const;
    const std::string &Message() const;

    EventLevel Level() const;
    const xUnitpp::LineInfo &LineInfo() const;

    friend std::string to_string(const TestEvent &event);

private:
    EventLevel level;
    xUnitAssert assert;
    std::string message;
    xUnitpp::LineInfo lineInfo;
};

}

#endif
