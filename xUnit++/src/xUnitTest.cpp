#include "xUnitTest.h"
#include "EventLevel.h"
#include "TestEventRecorder.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

xUnitTest::xUnitTest(std::function<void()> &&test, std::string &&name, int testInstance, std::string &&params,
                     const std::string &suite, AttributeCollection &&attributes, Time::Duration timeLimit,
                     std::string &&filename, int line, const std::vector<std::shared_ptr<TestEventRecorder>> &testEventRecorders)
    : test(std::move(test))
    , testDetails(std::move(name), testInstance, std::move(params), suite, std::move(attributes), timeLimit, std::move(filename), line)
    , testEventRecorders(testEventRecorders)
    , failureEventLogged(false)
{
}

const TestDetails &xUnitTest::TestDetails() const
{
    return testDetails;
}

TestResult xUnitTest::Run()
{
    for (auto &recorder : testEventRecorders)
    {
        recorder->Tie([&](TestEvent &&evt) { AddEvent(std::move(evt)); });
    }

    testStart = Time::Clock::now();

    try
    {
        test();
    }
    catch (const xUnitAssert &assert)
    {
        AddEvent(TestEvent(EventLevel::Assert, assert));
    }
    catch (const std::exception &e)
    {
        AddEvent(TestEvent(e));
    }
    catch (...)
    {
        AddEvent(TestEvent(EventLevel::Fatal, "Unknown exception caught: test has crashed."));
    }

    testStop = Time::Clock::now();

    return failureEventLogged ? TestResult::Failure : TestResult::Success;
}

Time::Duration xUnitTest::Duration() const
{
    return Time::ToDuration(testStop - testStart);
}

void xUnitTest::AddEvent(TestEvent &&evt)
{
    std::lock_guard<std::mutex> lock(eventLock);

    testEvents.push_back(std::move(evt));

    if (testEvents.back().GetIsFailure())
    {
        failureEventLogged = true;
    }
}

const std::vector<TestEvent> &xUnitTest::TestEvents() const
{
    return testEvents;
}

}
