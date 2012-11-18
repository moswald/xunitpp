#include "xUnitAssert.h"
#include <cmath>

namespace xUnitpp
{

xUnitAssert xUnitAssert::None()
{
    return xUnitAssert("", xUnitpp::LineInfo());
}

xUnitAssert::xUnitAssert(std::string &&call, xUnitpp::LineInfo &&lineInfo)
    : lineInfo(std::move(lineInfo))
    , call(std::move(call))
    , userMessage(std::make_shared<std::stringstream>())
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

const std::string &xUnitAssert::Call() const
{
    return call;
}

std::string xUnitAssert::UserMessage() const
{
    return userMessage->str();
}

const std::string &xUnitAssert::CustomMessage() const
{
    return customMessage;
}

const std::string &xUnitAssert::Expected() const
{
    return expected;
}

const std::string &xUnitAssert::Actual() const
{
    return actual;
}

const LineInfo &xUnitAssert::LineInfo() const
{
    return lineInfo;
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

xUnitFailure Assert::Contains(const std::string &actualString, const std::string &value, LineInfo &&lineInfo) const
{
    if (actualString.find(value) == std::string::npos)
    {
        return OnFailure(std::move(xUnitAssert(callPrefix + "Contains", std::move(lineInfo))
            .Expected(std::string(value))    // can't assume actualString or value can be moved
            .Actual(std::string(actualString))));
    }

    return OnSuccess();
}

Assert::Assert(std::string &&callPrefix, std::function<void (const xUnitAssert &)> &&onFailure)
    : callPrefix(std::move(callPrefix))
    , handleFailure(std::move(onFailure))
{
}

}
