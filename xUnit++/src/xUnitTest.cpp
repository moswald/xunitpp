#include "xUnitTest.h"
#include "ITestEventSource.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

xUnitTest::xUnitTest(std::function<void()> test, const std::string &name, const std::string &suite,
                     const AttributeCollection &attributes, Time::Duration timeLimit,
                     const std::string &filename, int line, const std::vector<std::shared_ptr<ITestEventSource>> &testEventSources)
    : test(test)
    , testDetails(name, suite, attributes, timeLimit, filename, line)
    , failureEventLogged(false)
    , testEventSources(testEventSources)
{
}

const TestDetails &xUnitTest::TestDetails() const
{
    return testDetails;
}

TestResult xUnitTest::Run()
{
    for (auto &source : testEventSources)
    {
        source->SetSink([&](TestEvent &&evt) { AddEvent(std::move(evt)); });
    }

    testStart = Time::Clock::now();

    try
    {
        test();
    }
    catch (const xUnitAssert &assert)
    {
        AddEvent(TestEvent(assert));
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

    testEvents.emplace_back(evt);

    if (evt.IsFailure())
    {
        failureEventLogged = true;
    }
}

const std::vector<TestEvent> &xUnitTest::TestEvents() const
{
    return testEvents;
}

}
