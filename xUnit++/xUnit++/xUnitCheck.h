#ifndef XUNITCHECK_H_
#define XUNITCHECK_H_

#include <functional>
#include <map>
#include <memory>
#include <thread>
#include "ITestEventSource.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

class Check : public Assert, public ITestEventSource
{
public:
    Check();

private:
    Check(const Check &) /* = delete */;
    Check(Check &&) /* = delete */;
    Check &operator =(Check) /* = delete */;

    virtual void SetSink(std::function<void(TestEvent &&)> sink) override;

private:
    std::map<std::thread::id, std::function<void(TestEvent &&)>> sinks;
};

}

#endif
