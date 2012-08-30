#include "xUnitAssert.h"

namespace
{
    std::string AssembleWhat(const std::string &call, const std::string &userMsg, const std::string &customMsg,
                             const std::string &expected, const std::string &actual)
    {
        std::string msg = "Assert." + call + "() failure";
        if (!userMsg.empty())
        {
            msg += ": " + userMsg;

            if (!customMsg.empty())
            {
                msg += "\n     " + customMsg;
            }
        }
        else
        {
            if (!customMsg.empty())
            {
                msg += ": " + customMsg;
            }
        }

        if (!expected.empty())
        {
            msg += "\n     Expected: " + expected;
            msg += "\n       Actual: " + actual;
        }

        msg += "\n";

        return msg;
    }
}

namespace xUnitpp
{

xUnitAssert::xUnitAssert(const std::string &call, const std::string &userMsg, const std::string &customMsg,
                         const std::string &expected, const std::string &actual, const xUnitpp::LineInfo &lineInfo)
    : base(AssembleWhat(call, userMsg, customMsg, expected, actual).c_str())
    , lineInfo(lineInfo)
{
}

xUnitAssert::xUnitAssert(const xUnitAssert &other)
    : base(other.what())
    , lineInfo(other.lineInfo)
{
}

const LineInfo &xUnitAssert::LineInfo() const
{
    return lineInfo;
}

double Assert::round(double value, size_t precision)
{
    if (value < 0)
    {
        return std::ceil((value - 0.5) * std::pow(10, precision)) / std::pow(10, precision);
    }
    else
    {
        return std::floor((value + 0.5) * std::pow(10, precision)) / std::pow(10, precision);
    }
}

void Assert::Equal(const std::string &expected, const std::string &actual, const std::string &msg, const LineInfo &lineInfo) const
{
    if (expected != actual)
    {
        throw xUnitAssert("Equal", msg, "", expected, actual, lineInfo);
    }
}

void Assert::Equal(const std::string &expected, const std::string &actual, const LineInfo &lineInfo) const
{
    Equal(expected, actual, std::string(""), lineInfo);
}

void Assert::Equal(const char *expected, const char *actual, const std::string &msg, const LineInfo &lineInfo) const
{
    Equal(std::string(expected), std::string(actual), msg, lineInfo);
}

void Assert::Equal(const char *expected, const char *actual, const LineInfo &lineInfo) const
{
    Equal(std::string(expected), std::string(actual), std::string(""), lineInfo);
}

void Assert::Equal(const std::string &expected, const char *actual, const std::string &msg, const LineInfo &lineInfo) const
{
    Equal(expected, std::string(actual), msg, lineInfo);
}

void Assert::Equal(const std::string &expected, const char *actual, const LineInfo &lineInfo) const
{
    Equal(expected, std::string(actual), std::string(""), lineInfo);
}

void Assert::Equal(float expected, float actual, int precision, const std::string &msg, const LineInfo &lineInfo) const
{
    Equal((double)expected, (double)actual, precision, msg, lineInfo);
}

void Assert::Equal(float expected, float actual, int precision, const LineInfo &lineInfo) const
{
    Equal(expected, actual, precision, "", lineInfo);
}

void Assert::Equal(double expected, double actual, int precision, const std::string &msg, const LineInfo &lineInfo) const
{
    auto er = round(expected, precision);
    auto ar = round(actual, precision);

    Equal(er, ar, [](double er, double ar) { return er == ar; }, msg, lineInfo);
}

void Assert::Equal(double expected, double actual, int precision, const LineInfo &lineInfo) const
{
    Equal(expected, actual, precision, "", lineInfo);
}

void Assert::Fail(const std::string &msg, const LineInfo &lineInfo) const
{
    throw xUnitAssert("Fail", msg, "", "", "", lineInfo);
}

void Assert::Fail(const LineInfo &lineInfo) const
{
    Fail("", lineInfo);
}

void Assert::False(bool b, const std::string &msg, const LineInfo &lineInfo) const
{
    if (b)
    {
        throw xUnitAssert("False", msg, "", "false", "true", lineInfo);
    }
}

void Assert::False(bool b, const LineInfo &lineInfo) const
{
    False(b, "", lineInfo);
}

void Assert::True(bool b, const std::string &msg, const LineInfo &lineInfo) const
{
    if (!b)
    {
        throw xUnitAssert("True", msg, "", "true", "false", lineInfo);
    }
}

void Assert::True(bool b, const LineInfo &lineInfo) const
{
    True(b, "", lineInfo);
}

void Assert::DoesNotContain(const char *actualString, const char *value, const std::string &msg, const LineInfo &lineInfo) const
{
    DoesNotContain(std::string(actualString), std::string(value), msg, lineInfo);
}

void Assert::DoesNotContain(const char *actualString, const char *value, const LineInfo &lineInfo) const
{
    DoesNotContain(actualString, value, "", lineInfo);
}

void Assert::DoesNotContain(const std::string &actualString, const char *value, const std::string &msg, const LineInfo &lineInfo) const
{
    DoesNotContain(actualString, std::string(value), msg, lineInfo);
}

void Assert::DoesNotContain(const std::string &actualString, const char *value, const LineInfo &lineInfo) const
{
    DoesNotContain(actualString, value, "", lineInfo);
}

void Assert::DoesNotContain(const std::string &actualString, const std::string &value, const std::string &msg, const LineInfo &lineInfo) const
{
    auto found = actualString.find(value);
    if (found != std::string::npos)
    {
        throw xUnitAssert("DoesNotContain", msg, "Found: \"" + value + "\" at position " + std::to_string(found) + ".", "", "", lineInfo);
    }
}

void Assert::DoesNotContain(const std::string &actualString, const std::string &value, const LineInfo &lineInfo) const
{
    DoesNotContain(actualString, value, "", lineInfo);
}

void Assert::Contains(const char *actualString, const char *value, const std::string &msg, const LineInfo &lineInfo) const
{
    Contains(std::string(actualString), std::string(value), msg, lineInfo);
}

void Assert::Contains(const char *actualString, const char *value, const LineInfo &lineInfo) const
{
    Contains(actualString, value, "", lineInfo);
}

void Assert::Contains(const std::string &actualString, const char *value, const std::string &msg, const LineInfo &lineInfo) const
{
    Contains(actualString, std::string(value), msg, lineInfo);
}

void Assert::Contains(const std::string &actualString, const char *value, const LineInfo &lineInfo) const
{
    Contains(actualString, value, "", lineInfo);
}

void Assert::Contains(const std::string &actualString, const std::string &value, const std::string &msg, const LineInfo &lineInfo) const
{
    if (actualString.find(value) == std::string::npos)
    {
        throw xUnitAssert("Contains", msg, "", actualString, value, lineInfo);
    }
}

void Assert::Contains(const std::string &actualString, const std::string &value, const LineInfo &lineInfo) const
{
    Contains(actualString, value, "", lineInfo);
}

}
