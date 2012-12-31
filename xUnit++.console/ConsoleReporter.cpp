#include "ConsoleReporter.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include "xUnit++/EventLevel.h"
#include "xUnit++/LineInfo.h"
#include "xUnit++/ITestDetails.h"
#include "xUnit++/ITestEvent.h"

#if defined (_WIN32)
#include <Windows.h>
#undef ReportEvent
#undef GetMessage

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

    template<typename T>
    xUnitpp::LineInfo GetSafeLineInfo(const T &t)
    {
        return xUnitpp::LineInfo(t.GetFile(), t.GetLine());
    }

    std::string FileAndLine(const xUnitpp::ITestDetails &td, const xUnitpp::LineInfo &lineInfo)
    {
        auto result = to_string(lineInfo);
        if (result.empty())
        {
            result = to_string(GetSafeLineInfo(td));
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

    std::string safestr(const char *s)
    {
        return s == nullptr ? "" : s;
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

        TestOutput(const xUnitpp::ITestDetails &td, bool verbose)
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
                    auto suite = safestr(testDetails.GetSuite());
                    if (!suite.empty())
                    {
                        std::cout << Fragment(Color::Suite, suite);
                        std::cout << Fragment(Color::Separator, TestSeparator);
                    }
                }

                auto fullName = safestr(testDetails.GetFullName());
                std::cout << Fragment(Color::TestName, fullName + "\n");

                for (auto &&msg : fragments)
                {
                    std::cout << msg;
                }
            }
        }

        void Skip(const std::string &reason)
        {
            fragments.emplace_back(Color::FileAndLine, to_string(xUnitpp::LineInfo(testDetails.GetFile(), testDetails.GetLine())));
            fragments.emplace_back(Color::Separator, ": ");
            fragments.emplace_back(Color::Skip, reason);
            fragments.emplace_back(Color::Default, "\n");
            skipped = true;
        }

        TestOutput &operator <<(const xUnitpp::ITestEvent &event)
        {
            if (event.GetIsFailure())
            {
                failed = true;
            }

            fragments.emplace_back(Color::FileAndLine, FileAndLine(testDetails, GetSafeLineInfo(event)));
            fragments.emplace_back(Color::Separator, ": ");
            fragments.emplace_back(to_color(event.GetLevel()), to_string(event.GetLevel()));
            fragments.emplace_back(Color::Separator, ": ");

            // unfortunately, this code should almost completely match the to_string(const TestEvent &)
            // function found in TestEvent.cpp, but is kept separate because of the color coding
            if (event.GetIsAssertType())
            {
                auto &&assert = event.GetAssertInterface();
                fragments.emplace_back(Color::Call, assert.GetCall() + std::string("()"));

                std::string message = " failure";

                std::string userMessage = assert.GetUserMessage();
                std::string customMessage = assert.GetCustomMessage();
                if (!userMessage.empty())
                {
                    message += ": " + userMessage;

                    if (!customMessage.empty())
                    {
                        message += "\n     " + customMessage;
                    }
                }
                else if (!customMessage.empty())
                {
                    message += ": " + customMessage;
                }
                else
                {
                    message += ".";
                }

                fragments.emplace_back(Color::Default, message);

                std::string expected = assert.GetExpected();
                std::string actual = assert.GetActual();
                if (!expected.empty() || !actual.empty())
                {
                    fragments.emplace_back(Color::Expected, "\n     Expected: ");
                    fragments.emplace_back(Color::Default, expected);
                    fragments.emplace_back(Color::Actual, "\n       Actual: ");
                    fragments.emplace_back(Color::Default, actual);
                }
            }
            else
            {
                std::string message = event.GetMessage();
                fragments.emplace_back(Color::Default, message);
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

        const xUnitpp::ITestDetails &TestDetails() const
        {
            return testDetails;
        }

    private:
        TestOutput(const TestOutput &) /* = delete */;
        TestOutput &operator =(TestOutput) /* = delete */;

    private:
        const xUnitpp::ITestDetails &testDetails;
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

    TestOutput &Cache(const xUnitpp::ITestDetails &td)
    {
        auto id = td.GetId();

        auto it = cache.find(id);
        if (it == cache.end())
        {
            cache.insert(std::make_pair(id, std::make_shared<TestOutput>(td, verbose)));
        }

        return *cache[id];
    }

    void Skip(const xUnitpp::ITestDetails &testDetails, const std::string &reason)
    {
        if (!sort)
        {
            Instant(Color::Skip, "\n[ Skipped ] ");

            auto suite = safestr(testDetails.GetSuite());
            if (!suite.empty())
            {
                Instant(Color::Suite, suite);
                Instant(Color::Separator, TestSeparator);
            }

            auto name = safestr(testDetails.GetName());
            Instant(Color::TestName, name + "\n");
            Instant(Color::FileAndLine, to_string(GetSafeLineInfo(testDetails)) + ": ");
            Instant(Color::Skip, reason);
            Instant(Color::Default, "\n");
        }
        else
        {
            Cache(testDetails).Skip(reason);
        }
    }

    void Finish(const xUnitpp::ITestDetails &td)
    {
        if (!sort)
        {
            auto it = cache.find(td.GetId());
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
                    auto lhsSuite = safestr(lhs->TestDetails().GetSuite());
                    auto rhsSuite = safestr(rhs->TestDetails().GetSuite());
                    if (lhsSuite != rhsSuite)
                    {
                        return lhsSuite < rhsSuite;
                    }

                    auto lhsName = lhs->TestDetails().GetName();
                    auto rhsName = rhs->TestDetails().GetName();
                    return lhsName < rhsName;
                });

            std::string curSuite = "";
            for (auto it = finalResults.begin(); it != finalResults.end(); ++it)
            {
                auto &result = *it;
                if (result->WillPrint())
                {
                    if (group)
                    {
                        auto suite = safestr(result->TestDetails().GetSuite());
                        if (curSuite != suite)
                        {
                            curSuite = suite;

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

void ConsoleReporter::ReportStart(const ITestDetails &)
{
}

void ConsoleReporter::ReportEvent(const ITestDetails &testDetails, const ITestEvent &evt)
{
    cache->Cache(testDetails) << evt;
}

void ConsoleReporter::ReportSkip(const ITestDetails &testDetails, const char *reason)
{
    cache->Skip(testDetails, reason);
}

void ConsoleReporter::ReportFinish(const ITestDetails &testDetails, long long nsTaken)
{
    cache->Cache(testDetails) << Time::Duration(nsTaken);
    cache->Finish(testDetails);
}

void ConsoleReporter::ReportAllTestsComplete(size_t testCount, size_t skipped, size_t failureCount, long long nsTotal)
{
    auto totalTime = Time::Duration(nsTotal);

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
