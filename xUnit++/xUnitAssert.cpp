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
                         const std::string &expected, const std::string &actual)
    : base(AssembleWhat(call, userMsg, customMsg, expected, actual).c_str())
{
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

void Assert::Equal(float expected, float actual, int precision, const std::string &msg) const
{
    Equal((double)expected, (double)actual, precision, msg);
}

void Assert::Equal(double expected, double actual, int precision, const std::string &msg) const
{
    auto er = round(expected, precision);
    auto ar = round(actual, precision);

    Equal(er, ar, [](double er, double ar) { return er == ar; }, msg);
}

void Assert::Fail(const std::string &msg) const
{
    throw xUnitAssert("Fail", msg, "", "", "");
}

void Assert::False(bool b, const std::string &msg) const
{
    if (b)
    {
        throw xUnitAssert("False", msg, "", "false", "true");
    }
}

void Assert::True(bool b, const std::string &msg) const
{
    if (!b)
    {
        throw xUnitAssert("True", msg, "", "true", "false");
    }
}

void Assert::DoesNotContain(const char *actualString, const char *value, const std::string &msg) const
{
    DoesNotContain(std::string(actualString), std::string(value), msg);
}

void Assert::DoesNotContain(const std::string &actualString, const char *value, const std::string &msg) const
{
    DoesNotContain(actualString, std::string(value), msg);
}

void Assert::DoesNotContain(const std::string &actualString, const std::string &value, const std::string &msg) const
{
    auto found = actualString.find(value);
    if (found != std::string::npos)
    {
        throw xUnitAssert("DoesNotContain", msg, "Found: \"" + value + "\" at position " + std::to_string(found) + ".", "", "");
    }
}

void Assert::Contains(const char *actualString, const char *value, const std::string &msg) const
{
    Contains(std::string(actualString), std::string(value), msg);
}

void Assert::Contains(const std::string &actualString, const char *value, const std::string &msg) const
{
    Contains(actualString, std::string(value), msg);
}

void Assert::Contains(const std::string &actualString, const std::string &value, const std::string &msg) const
{
    if (actualString.find(value) == std::string::npos)
    {
        throw xUnitAssert("Contains", msg, "", actualString, value);
    }
}

}
