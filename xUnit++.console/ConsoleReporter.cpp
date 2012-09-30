#include "ConsoleReporter.h"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"
#include "xUnit++/TestEvent.h"


namespace
{
    std::string FileAndLine(const xUnitpp::TestDetails &td, const xUnitpp::LineInfo &lineInfo)
    {
        auto result = to_string(lineInfo);
        if (result.empty())
        {
            result = to_string(td.LineInfo);
        }

        return result;
    }

    class CachedOutput
    {
        typedef std::map<int, std::shared_ptr<CachedOutput>> OutputCache;

    public:
        CachedOutput(const std::string &name)
            : name(name)
        {
        }

        ~CachedOutput()
        {
            if (!messages.empty())
            {
                std::cout << "\n[" << name << "]\n";

                for (const auto &msg : messages)
                {
                    std::cout << msg << std::endl;
                }
            }
        }

        static void Instant(const std::string &output)
        {
            std::cout << output;
        }

        static CachedOutput &Cache(const xUnitpp::TestDetails &td)
        {
            auto &cache = Cache();

            auto it = cache.find(td.Id);
            if (it == cache.end())
            {
                cache.insert(std::make_pair(td.Id, std::make_shared<CachedOutput>(td.Name)));
            }

            return *cache[td.Id];
        }

        static void Finish(const xUnitpp::TestDetails &td)
        {
            auto &cache = Cache();

            auto it = cache.find(td.Id);
            if (it != cache.end())
            {
                cache.erase(it);
            }
        }

        CachedOutput &operator <<(const std::string &output)
        {
            messages.push_back(output);
            return *this;
        }

    private:
        static OutputCache &Cache()
        {
            static OutputCache cache;
            return cache;
        }

        std::string name;
        std::vector<std::string> messages;
    };
}

namespace xUnitpp
{

ConsoleReporter::ConsoleReporter(bool verbose, bool veryVerbose)
    : mVerbose(verbose)
    , mVeryVerbose(veryVerbose)
{
}

void ConsoleReporter::ReportStart(const TestDetails &testDetails)
{
    if (mVeryVerbose)
    {
        CachedOutput::Instant("Starting test " + testDetails.Name + ".");
    }
}

void ConsoleReporter::ReportEvent(const TestDetails &testDetails, const TestEvent &evt)
{
    CachedOutput::Cache(testDetails) << (FileAndLine(testDetails, evt.LineInfo()) +
        ": " + to_string(evt.Level()) + ": " + to_string(evt));
}

void ConsoleReporter::ReportSkip(const TestDetails &testDetails, const std::string &reason)
{
    CachedOutput::Instant(FileAndLine(testDetails, LineInfo::empty()) +
        ": skipping " + testDetails.Name + ": " + reason);
}

void ConsoleReporter::ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken)
{
    if (mVerbose)
    {
        auto ms = Time::ToMilliseconds(timeTaken);
        CachedOutput::Cache(testDetails) << (testDetails.Name + ": Completed in " +
            (ms.count() == 0 ? (std::to_string(timeTaken.count()) + " nanoseconds.\n") : (std::to_string(ms.count()) + " milliseconds.\n")));
    }

    CachedOutput::Finish(testDetails);
}

void ConsoleReporter::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, Time::Duration totalTime)
{
    std::string total = std::to_string(testCount) + " tests, ";
    std::string failures = std::to_string(failureCount) + " failed, ";
    std::string skips = std::to_string(skipped) + " skipped.";

    std::string header;

    if (failureCount > 0)
    {
        header = "\nFAILURE: ";
    }
    else if (skipped > 0)
    {
        header = "\nWARNING: ";
    }
    else
    {
        header = "\nSuccess: ";
    }

    CachedOutput::Instant(header + total + failures + skips);

    header = "Test time: ";

    auto ms = Time::ToMilliseconds(totalTime);

    if (ms.count() > 500)
    {
        CachedOutput::Instant(header + std::to_string(Time::ToSeconds(totalTime).count()) + " seconds.");
    }
    else
    {
        CachedOutput::Instant(header + std::to_string(ms.count()) + " milliseconds.");
    }
}

}
