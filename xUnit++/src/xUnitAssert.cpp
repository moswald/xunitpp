#include "xUnitAssert.h"

namespace
{
    std::string AssembleWhat(const std::string &call, const std::vector<std::string> &userMsg, const std::string &customMsg,
                             const std::string &expected, const std::string &actual)
    {
        std::string msg = call + "() failure";
        if (!userMsg.empty())
        {
            msg += ": ";
            
            for (const auto &s : userMsg)
            {
                msg += s;
            }

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

const xUnitAssert &xUnitAssert::None()
{
    static xUnitAssert none("", LineInfo::empty());
    return none;
}

xUnitAssert::xUnitAssert(const std::string &call, const xUnitpp::LineInfo &lineInfo)
    : lineInfo(lineInfo)
    , call(call)
{
}

xUnitAssert &xUnitAssert::CustomMessage(const std::string &message)
{
    customMessage = message;
    return *this;
}

xUnitAssert &xUnitAssert::Expected(const std::string &str)
{
    expected = str;
    return *this;
}

xUnitAssert &xUnitAssert::Actual(const std::string &str)
{
    actual = str;
    return *this;
}

const LineInfo &xUnitAssert::LineInfo() const
{
    return lineInfo;
}

const char *xUnitAssert::what() const
{
    if (whatMessage.empty())
    {
        whatMessage = AssembleWhat(call, userMessage, customMessage, expected, actual);
    }

    return whatMessage.c_str();
}

xUnitFailure::xUnitFailure()
    : OnFailureComplete([](const xUnitAssert &){})
    , assert(xUnitAssert::None())
    , refCount(*(new int(0)))
{
}

xUnitFailure::xUnitFailure(xUnitAssert assert, std::function<void(const xUnitAssert &)> onFailureComplete)
    : OnFailureComplete(onFailureComplete)
    , assert(assert)
    , refCount(*(new int(1)))
{
}

xUnitFailure::xUnitFailure(const xUnitFailure &other)
    : OnFailureComplete(other.OnFailureComplete)
    , assert(other.assert)
    , refCount(other.refCount)
{
    refCount++;
}

xUnitFailure::~xUnitFailure()
{
    if (!--refCount)
    {
        delete &refCount;

        // http://cpp-next.com/archive/2012/08/evil-or-just-misunderstood/
        // http://akrzemi1.wordpress.com/2011/09/21/destructors-that-throw/
        // throwing destructors aren't Evil, just misunderstood
        OnFailureComplete(assert);
    } 
}

xUnitFailure xUnitFailure::None()
{
    return xUnitFailure();
}

xUnitFailure Assert::OnSuccess() const
{
    return xUnitFailure::None();
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

xUnitFailure Assert::Equal(const std::string &expected, const std::string &actual, const LineInfo &lineInfo) const
{
    if (expected != actual)
    {
        return OnFailure(xUnitAssert(callPrefix + "Equal", lineInfo)
            .Expected(expected)
            .Actual(actual));
    }

    return OnSuccess();
}

xUnitFailure Assert::Equal(const char *expected, const char *actual, const LineInfo &lineInfo) const
{
    return Equal(std::string(expected), std::string(actual), lineInfo);
}

xUnitFailure Assert::Equal(const std::string &expected, const char *actual, const LineInfo &lineInfo) const
{
    return Equal(expected, std::string(actual), lineInfo);
}

xUnitFailure Assert::Equal(float expected, float actual, int precision, const LineInfo &lineInfo) const
{
    return Equal((double)expected, (double)actual, precision, lineInfo);
}

xUnitFailure Assert::Equal(double expected, double actual, int precision, const LineInfo &lineInfo) const
{
    auto er = round(expected, precision);
    auto ar = round(actual, precision);

    return Equal(er, ar, [](double er, double ar) { return er == ar; }, lineInfo);
}

xUnitFailure Assert::Fail(const LineInfo &lineInfo) const
{
    return OnFailure(xUnitAssert(callPrefix + "Fail", lineInfo));
}

xUnitFailure Assert::False(bool b, const LineInfo &lineInfo) const
{
    if (b)
    {
        return OnFailure(xUnitAssert(callPrefix + "False", lineInfo).Expected("false").Actual("true"));
    }

    return OnSuccess();
}

xUnitFailure Assert::True(bool b, const LineInfo &lineInfo) const
{
    if (!b)
    {
        return OnFailure(xUnitAssert(callPrefix + "True", lineInfo).Expected("true").Actual("false"));
    }

    return OnSuccess();
}

xUnitFailure Assert::DoesNotContain(const char *actualString, const char *value, const LineInfo &lineInfo) const
{
    return DoesNotContain(std::string(actualString), std::string(value), lineInfo);
}

xUnitFailure Assert::DoesNotContain(const std::string &actualString, const char *value, const LineInfo &lineInfo) const
{
    return DoesNotContain(actualString, std::string(value), lineInfo);
}

xUnitFailure Assert::DoesNotContain(const std::string &actualString, const std::string &value, const LineInfo &lineInfo) const
{
    auto found = actualString.find(value);
    if (found != std::string::npos)
    {
        return OnFailure(xUnitAssert(callPrefix + "DoesNotContain", lineInfo).CustomMessage("Found: \"" + value + "\" at position " + std::to_string(found) + "."));
    }

    return OnSuccess();
}

xUnitFailure Assert::Contains(const char *actualString, const char *value, const LineInfo &lineInfo) const
{
    return Contains(std::string(actualString), std::string(value), lineInfo);
}

xUnitFailure Assert::Contains(const std::string &actualString, const char *value, const LineInfo &lineInfo) const
{
    return Contains(actualString, std::string(value), lineInfo);
}

xUnitFailure Assert::Contains(const std::string &actualString, const std::string &value, const LineInfo &lineInfo) const
{
    if (actualString.find(value) == std::string::npos)
    {
        return OnFailure(xUnitAssert(callPrefix + "Contains", lineInfo).Expected(actualString).Actual(value));
    }

    return OnSuccess();
}

Assert::Assert(const std::string &callPrefix, std::function<xUnitFailure(xUnitAssert)> onFailure)
    : callPrefix(callPrefix)
    , OnFailure(onFailure)
{
}

}
