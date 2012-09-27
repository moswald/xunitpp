#ifndef XUNITLOG_H_
#define XUNITLOG_H_

#include <functional>
#include <memory>
#include <sstream>
#include "TestEvent.h"

namespace xUnitpp
{

class TestEventRecorder;

class Log
{
    class Logger
    {
        class Message
        {
        public:
            Message(std::function<void(const std::string &)> recordMessage);
            Message(const Message &other);
            ~Message();

            template<typename T>
            Message &operator <<(const T &value)
            {
                message << value;
                return *this;
            }

        private:
            Message &operator =(Message) /* = delete */;

        private:
            size_t &refCount;
            std::function<void(const std::string &)> recordMessage;
            std::stringstream message;
        };
    public:
        Logger(std::function<void(const std::string &)> recordMessage);

        template<typename T>
        Message operator <<(const T &value) const
        {
            return (Message(recordMessage) << value);
        }

    private:
        std::function<void(const std::string &)> recordMessage;
    };

public:
    Log(const TestEventRecorder &recorder);

    const Logger Debug;
    const Logger Info;
    const Logger Warn;

private:
    Log(const Log &) /* = delete */;
    Log(Log &&) /* = delete */;
    Log &operator =(Log) /* = delete */;
};

}

#endif
