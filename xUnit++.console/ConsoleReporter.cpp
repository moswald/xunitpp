#include "ConsoleReporter.h"
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <unordered_map>

#if defined (_WIN32)
#include <Windows.h>
#undef ReportEvent

namespace
{
    class ResetConsoleColors
    {
    public:
        ResetConsoleColors()
            : stdOut(GetStdHandle(STD_OUTPUT_HANDLE))
        {
            CONSOLE_SCREEN_BUFFER_INFO info;
            GetConsoleScreenBufferInfo(stdOut, &info);
            attributes = info.wAttributes;
        }

        ~ResetConsoleColors()
        {
            Reset();
        }

        void Reset()
        {
            SetConsoleTextAttribute(stdOut, attributes);
        }

    private:
        HANDLE stdOut;
        WORD attributes;
    } resetConsoleColors;
}

#else
#endif

#include "xUnit++/LineInfo.h"
#include "xUnit++/TestDetails.h"
#include "xUnit++/TestEvent.h"

namespace
{
    static const std::string TestSeparator = "\n            ";

    enum class Color : unsigned short
    {
        Default = 65535,

        Black = 0,
        DarkBlue,
        DarkGreen,
        DarkCyan,
        DarkRed,
        DarkMagenta,
        DarkYellow,
        LightGray,
        DarkGray,
        Blue,
        Green,
        Cyan,
        Red,
        Magenta,
        Yellow,
        White,

        Debug = DarkMagenta,
        Info = DarkCyan,
        Warning = Yellow,
        Check = Red,
        Assert = Red,
        Skip = Yellow,
        Suite = White,
        Separator = DarkGray,
        TestName = White,
        FileAndLine = Default,
        Success = Green,
        Failure = Red,
        TimeSummary = DarkGray,
        Call = White,
        Expected = Cyan,
        Actual = Cyan,

        // this value is to match the Win32 value
        // there is a special test in to_ansicode
        Fatal = (0x40 + White),
    };

    Color to_color(xUnitpp::EventLevel level)
    {
        switch (level)
        {
        case xUnitpp::EventLevel::Debug:
            return Color::Debug;
        case xUnitpp::EventLevel::Info:
            return Color::Info;
        case xUnitpp::EventLevel::Warning:
            return Color::Warning;
        case xUnitpp::EventLevel::Check:
            return Color::Check;
        case xUnitpp::EventLevel::Assert:
            return Color::Assert;
        case xUnitpp::EventLevel::Fatal:
            return Color::Fatal;
        }

        return Color::Default;
    }

    std::string to_ansicode(Color color)
    {
        static const std::string codes[] =
        {
            "\033[0;30m",
            "\033[0;34m",
            "\033[0;32m",
            "\033[0;36m",
            "\033[0;31m",
            "\033[0;35m",
            "\033[0;33m",
            "\033[0;37m",
            "\033[0;1;30m", // the 0 at the start clears the background color
            "\033[0;1;34m", // then the 1 sets the color to bold
            "\033[0;1;32m",
            "\033[0;1;36m",
            "\033[0;1;31m",
            "\033[0;1;35m",
            "\033[0;1;33m",
            "\033[0;1;37m",
        };

        if (color == Color::Fatal)
        {
            return "\033[1;37;41m";
        }

        if (color != Color::Default)
        {
            return codes[(int)color];
        }

        return "\033[m";
    }

    std::string FileAndLine(const xUnitpp::TestDetails &td, const xUnitpp::LineInfo &lineInfo)
    {
        auto result = to_string(lineInfo);
        if (result.empty())
        {
            result = to_string(td.LineInfo);
        }

        return result;
    }

    template<typename TStream>
    TStream &operator <<(TStream &stream, Color color)
    {
#if defined (_WIN32)
        stream.flush();
        if (color != Color::Default)
        {
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (unsigned short)color);
        }
        else
        {
            resetConsoleColors.Reset();
        }
#else
        stream << to_ansicode(color);
#endif

        return stream;
    }
}

namespace xUnitpp
{

class ConsoleReporter::ReportCache
{
    class TestOutput
    {
    public:
        struct Fragment
        {
            Fragment(Color color, const std::string &message)
                : color(color)
                , message(message)
            {
            }

            friend std::ostream &operator <<(std::ostream &stream, const Fragment &fragment)
            {
                return stream << fragment.color << fragment.message;
            }

            Color color;
            std::string message;
        };

        TestOutput(const xUnitpp::TestDetails &td, bool verbose)
            : testDetails(td)
            , failed(false)
            , verbose(verbose)
            , skipped(false)
        {
        }

        bool WillPrint() const
        {
            return failed || verbose || !fragments.empty();
        }

        void Print(bool grouped)
        {
            if (WillPrint())
            {
                if (!grouped)
                {
                    std::cout << "\n";
                }

                if (failed)
                {
                    std::cout << Fragment(Color::Failure, "[ Failure ] ");
                }
                else if (skipped)
                {
                    std::cout << Fragment(Color::Skip, "[ Skipped ] ");
                }
                else
                {
                    std::cout << Fragment(Color::Success, "[ Success ] ");
                }

                if (!grouped)
                {
                    if (!testDetails.Suite.empty())
                    {
                        std::cout << Fragment(Color::Suite, testDetails.Suite);
                        std::cout << Fragment(Color::Separator, TestSeparator);
                    }
                }

                std::cout << Fragment(Color::TestName, testDetails.FullName() + "\n");

                for (auto &&msg : fragments)
                {
                    std::cout << msg;
                }
            }
        }

        void Skip(const std::string &reason)
        {
            fragments.emplace_back(Color::FileAndLine, to_string(testDetails.LineInfo));
            fragments.emplace_back(Color::Separator, ": ");
            fragments.emplace_back(Color::Skip, reason);
            fragments.emplace_back(Color::Default, "\n");
            skipped = true;
        }

        TestOutput &operator <<(const xUnitpp::TestEvent &event)
        {
            if (event.IsFailure())
            {
                failed = true;
            }

            fragments.emplace_back(Color::FileAndLine, FileAndLine(testDetails, event.LineInfo()));
            fragments.emplace_back(Color::Separator, ": ");
            fragments.emplace_back(to_color(event.Level()), to_string(event.Level()));
            fragments.emplace_back(Color::Separator, ": ");

            // unfortunately, this code should almost completely match the to_string(const TestEvent &)
            // function found in TestEvent.cpp, but is kept separate because of the color coding
            if (event.IsAssertType())
            {
                auto &&assert = event.Assert();

                fragments.emplace_back(Color::Call, assert.Call() + "()");

                std::string message = " failure";

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

                fragments.emplace_back(Color::Default, message);

                if (!assert.Expected().empty() || !assert.Actual().empty())
                {
                    fragments.emplace_back(Color::Expected, "\n     Expected: ");
                    fragments.emplace_back(Color::Default, assert.Expected());
                    fragments.emplace_back(Color::Actual, "\n       Actual: ");
                    fragments.emplace_back(Color::Default, assert.Actual());
                }
            }
            else
            {
                fragments.emplace_back(Color::Default, event.Message());
            }

            fragments.emplace_back(Color::Default, "\n");
            return *this;
        }

        TestOutput &operator <<(xUnitpp::Time::Duration time)
        {
            if (verbose)
            {
                fragments.emplace_back(Color::TimeSummary, "Test completed in " + xUnitpp::Time::to_string(time) + ".\n");
            }

            return *this;
        }

        const xUnitpp::TestDetails &TestDetails() const
        {
            return testDetails;
        }

    private:
        TestOutput(const TestOutput &) /* = delete */;
        TestOutput &operator =(TestOutput) /* = delete */;

    private:
        const xUnitpp::TestDetails &testDetails;
        std::vector<Fragment> fragments;
        bool failed;
        bool verbose;
        bool skipped;
    };

    typedef std::unordered_map<int, std::shared_ptr<TestOutput>> OutputCache;

public:
    ReportCache(bool verbose, bool sort, bool group)
        : verbose(verbose)
        , sort(sort)
        , group(group)
    {
    }

    void Instant(Color color, const std::string &message)
    {
        std::cout << TestOutput::Fragment(color, message);
    }

    TestOutput &Cache(const xUnitpp::TestDetails &td)
    {
        auto it = cache.find(td.Id);
        if (it == cache.end())
        {
            cache.insert(std::make_pair(td.Id, std::make_shared<TestOutput>(td, verbose)));
        }

        return *cache[td.Id];
    }

    void Skip(const xUnitpp::TestDetails &testDetails, const std::string &reason)
    {
        if (!sort)
        {
            Instant(Color::Skip, "\n[ Skipped ] ");

            if (!testDetails.Suite.empty())
            {
                Instant(Color::Suite, testDetails.Suite);
                Instant(Color::Separator, TestSeparator);
            }

            Instant(Color::TestName, testDetails.Name + "\n");
            Instant(Color::FileAndLine, to_string(testDetails.LineInfo) + ": ");
            Instant(Color::Skip, reason);
            Instant(Color::Default, "\n");
        }
        else
        {
            Cache(testDetails).Skip(reason);
        }
    }

    void Finish(const xUnitpp::TestDetails &td)
    {
        if (!sort)
        {
            auto it = cache.find(td.Id);
            if (it != cache.end())
            {
                it->second->Print(false);
                cache.erase(it);
            }
        }
    }

    void Finish()
    {
        if (sort)
        {
            std::vector<std::shared_ptr<TestOutput>> finalResults;
            finalResults.reserve(cache.size());

            for (auto x : cache)
            {
                finalResults.push_back(x.second);
            }

            std::sort(finalResults.begin(), finalResults.end(),
                [](const std::shared_ptr<TestOutput> &lhs, const std::shared_ptr<TestOutput> &rhs)
                {
                    if (lhs->TestDetails().Suite != rhs->TestDetails().Suite)
                    {
                        return lhs->TestDetails().Suite < rhs->TestDetails().Suite;
                    }

                    return lhs->TestDetails().Name < rhs->TestDetails().Name;
                });

            std::string curSuite = "";
            for (auto it = finalResults.begin(); it != finalResults.end(); ++it)
            {
                auto &result = *it;
                if (result->WillPrint())
                {
                    if (group)
                    {
                        if (curSuite != result->TestDetails().Suite)
                        {
                            curSuite = result->TestDetails().Suite;

                            std::string sep(curSuite.length() + 4, '=');
                            std::cout << TestOutput::Fragment(Color::Suite, "\n\n" + sep + "\n[ " + curSuite + " ]\n" + sep + "\n");
                        }
                    }

                    result->Print(group);
                }
            }
        }
    }

private:
    OutputCache cache;
    bool verbose;
    bool sort;
    bool group;
};

ConsoleReporter::ConsoleReporter(bool verbose, bool sort, bool group)
    : cache(new ReportCache(verbose, sort, group))
{
    //std::cout.sync_with_stdio(false);
}

void ConsoleReporter::ReportStart(const TestDetails &)
{
}

void ConsoleReporter::ReportEvent(const TestDetails &testDetails, const TestEvent &evt)
{
    cache->Cache(testDetails) << evt;
}

void ConsoleReporter::ReportSkip(const TestDetails &testDetails, const std::string &reason)
{
    cache->Skip(testDetails, reason);
}

void ConsoleReporter::ReportFinish(const TestDetails &testDetails, Time::Duration timeTaken)
{
    cache->Cache(testDetails) << timeTaken;
    cache->Finish(testDetails);
}

void ConsoleReporter::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, Time::Duration totalTime)
{
    cache->Finish();

    Color failColor = Color::Default;
    Color skipColor = Color::Default;
    if (failureCount > 0)
    {
        failColor = Color::Failure;
        cache->Instant(failColor, "\nFAILURE");
    }
    else if (skipped > 0)
    {
        skipColor = Color::Warning;
        cache->Instant(skipColor, "\nWARNING");
    }
    else
    {
        cache->Instant(Color::Success, "\nSuccess");
    }

    cache->Instant(Color::Default, ": " + std::to_string(testCount) + " tests, ");
    cache->Instant(failColor, std::to_string(failureCount) + " failed");
    cache->Instant(Color::Default, ", ");
    cache->Instant(skipColor, std::to_string(skipped) + " skipped");
    cache->Instant(Color::Default, ".");

    std::string report = "\nTest time: ";

    auto ms = Time::ToMilliseconds(totalTime);

    if (ms.count() > 500)
    {
        report += std::to_string(Time::ToSeconds(totalTime).count()) + " seconds.";
    }
    else
    {
        report += std::to_string(ms.count()) + " milliseconds.";
    }

    cache->Instant(Color::TimeSummary, report);
    cache->Instant(Color::Default, "\n");
}

}
