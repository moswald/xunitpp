#ifndef TESTEVENT_H_
#define TESTEVENT_H_

#include <exception>
#include <string>
#include "LineInfo.h"

namespace xUnitpp
{

class xUnitAssert;

enum class EventLevel
{
    Debug,
    Info,
    Warning,
    Check,
    Assert,
    Fatal
};

class TestEvent
{
public:
    TestEvent(EventLevel level, const std::string &message);
    TestEvent(const xUnitAssert &assert);
    TestEvent(const std::exception &e);

    bool IsFailure() const;

    EventLevel Level() const;
    const xUnitpp::LineInfo &LineInfo() const;

    const std::string &LevelString() const;
    const std::string &ToString() const;

private:
    EventLevel level;
    std::string message;
    xUnitpp::LineInfo lineInfo;
};

}

#endif
