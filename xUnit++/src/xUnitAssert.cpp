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

        return msg;
    }
}

namespace xUnitpp
{

const xUnitAssert &xUnitAssert::None()
{
    static xUnitAssert none("", xUnitpp::LineInfo());
    return none;
}

xUnitAssert::xUnitAssert(std::string &&call, xUnitpp::LineInfo &&lineInfo)
    : lineInfo(std::move(lineInfo))
    , call(std::move(call))
{
}

xUnitAssert &xUnitAssert::CustomMessage(std::string &&message)
{
    customMessage = std::move(message);
    return *this;
}

xUnitAssert &xUnitAssert::Expected(std::string &&str)
{
    expected = std::move(str);
    return *this;
}

xUnitAssert &xUnitAssert::Actual(std::string &&str)
{
    actual = std::move(str);
    return *this;
}

const LineInfo &xUnitAssert::LineInfo() const
{
    return lineInfo;
}

const char *xUnitAssert::what() const noexcept(true)
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

xUnitFailure::xUnitFailure(xUnitAssert &&assert, std::function<void(const xUnitAssert &)> onFailureComplete)
    : OnFailureComplete(onFailureComplete)
    , assert(std::move(assert))
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

xUnitFailure::~xUnitFailure() noexcept(false)
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

xUnitFailure Assert::OnFailure(xUnitAssert &&assert) const
{
    return xUnitFailure(std::move(assert), handleFailure);
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

xUnitFailure Assert::Equal(const std::string &expected, const std::string &actual, LineInfo &&lineInfo) const
{
    if (expected != actual)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "Equal", std::move(lineInfo))
            .Expected(std::string(expected))    // can't assume expected or actual are allowed to be moved
            .Actual(std::string(actual))));
    }

    return OnSuccess();
}

xUnitFailure Assert::Equal(const char *expected, const char *actual, LineInfo &&lineInfo) const
{
    return Equal(std::string(expected), std::string(actual), std::move(lineInfo));
}

xUnitFailure Assert::Equal(const char *expected, const std::string &actual, LineInfo &&lineInfo) const
{
    return Equal(std::string(expected), actual, std::move(lineInfo));
}

xUnitFailure Assert::Equal(const std::string &expected, const char *actual, LineInfo &&lineInfo) const
{
    return Equal(expected, std::string(actual), std::move(lineInfo));
}

xUnitFailure Assert::Equal(float expected, float actual, int precision, LineInfo &&lineInfo) const
{
    return Equal((double)expected, (double)actual, precision, std::move(lineInfo));
}

xUnitFailure Assert::Equal(double expected, double actual, int precision, LineInfo &&lineInfo) const
{
    auto er = round(expected, precision);
    auto ar = round(actual, precision);

    return Equal(er, ar, [](double er, double ar) { return er == ar; }, std::move(lineInfo));
}

xUnitFailure Assert::NotEqual(const std::string &expected, const std::string &actual, LineInfo &&lineInfo) const
{
    if (expected == actual)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "NotEqual", std::move(lineInfo))
            .Expected(std::string(expected))    // can't assume expected or actual are allowed to be moved
            .Actual(std::string(actual))));
    }

    return OnSuccess();
}

xUnitFailure Assert::NotEqual(const char *expected, const char *actual, LineInfo &&lineInfo) const
{
    return NotEqual(std::string(expected), std::string(actual), std::move(lineInfo));
}

xUnitFailure Assert::NotEqual(const char *expected, const std::string &actual, LineInfo &&lineInfo) const
{
    return NotEqual(std::string(expected), actual, std::move(lineInfo));
}

xUnitFailure Assert::NotEqual(const std::string &expected, const char *actual, LineInfo &&lineInfo) const
{
    return NotEqual(expected, std::string(actual), std::move(lineInfo));
}

xUnitFailure Assert::Fail(LineInfo &&lineInfo) const
{
    return OnFailure(std::move(xUnitAssert(callPrefix + "Fail", std::move(lineInfo))));
}

xUnitFailure Assert::False(bool b, LineInfo &&lineInfo) const
{
    if (b)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "False", std::move(lineInfo)).Expected("false").Actual("true")));
    }

    return OnSuccess();
}

xUnitFailure Assert::True(bool b, LineInfo &&lineInfo) const
{
    if (!b)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "True", std::move(lineInfo)).Expected("true").Actual("false")));
    }

    return OnSuccess();
}

xUnitFailure Assert::DoesNotContain(const char *actualString, const char *value, LineInfo &&lineInfo) const
{
    const auto a = std::string(actualString);
    const auto v = std::string(value);
    return DoesNotContain(a, v, std::move(lineInfo));
}

xUnitFailure Assert::DoesNotContain(const char *actualString, const std::string &value, LineInfo &&lineInfo) const
{
    const auto a = std::string(actualString);
    return DoesNotContain(a, value, std::move(lineInfo));
}

xUnitFailure Assert::DoesNotContain(const std::string &actualString, const char *value, LineInfo &&lineInfo) const
{
    const auto v = std::string(value);
    return DoesNotContain(actualString, v, std::move(lineInfo));
}

xUnitFailure Assert::DoesNotContain(const std::string &actualString, const std::string &value, LineInfo &&lineInfo) const
{
    auto found = actualString.find(value);
    if (found != std::string::npos)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "DoesNotContain", std::move(lineInfo))
            .CustomMessage("Found: \"" + value + "\" at position " + ToString(found) + ".")));
    }

    return OnSuccess();
}

xUnitFailure Assert::Contains(const char *actualString, const char *value, LineInfo &&lineInfo) const
{
    const auto a = std::string(actualString);
    const auto v = std::string(value);
    return Contains(a, v, std::move(lineInfo));
}

xUnitFailure Assert::Contains(const char *actualString, const std::string &value, LineInfo &&lineInfo) const
{
    const auto a = std::string(actualString);
    return Contains(a, value, std::move(lineInfo));
}

xUnitFailure Assert::Contains(const std::string &actualString, const char *value, LineInfo &&lineInfo) const
{
    const auto v = std::string(value);
    return Contains(actualString, v, std::move(lineInfo));
}

xUnitFailure Assert::Contains(const std::string &actualString, const std::string &value, LineInfo &&lineInfo) const
{
    if (actualString.find(value) == std::string::npos)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "Contains", std::move(lineInfo))
            .Expected(std::string(actualString))    // can't assume actualString or value can be moved
            .Actual(std::string(value))));
    }

    return OnSuccess();
}

Assert::Assert(std::string &&callPrefix, std::function<void (const xUnitAssert &)> &&onFailure)
    : callPrefix(std::move(callPrefix))
    , handleFailure(std::move(onFailure))
{
}

}
