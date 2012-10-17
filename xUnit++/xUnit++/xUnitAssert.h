#ifndef XUNITASSERT_H_
#define XUNITASSERT_H_

#if defined(_MSC_VER)
#define _ALLOW_KEYWORD_MACROS
#define noexcept(x)
#endif

#include <algorithm>
#include <exception>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>
#include "LineInfo.h"
#include "xUnitToString.h"

namespace xUnitpp
{

class xUnitAssert : public std::exception
{
    typedef std::exception base;

public:
    xUnitAssert(std::string &&call, const LineInfo &lineInfo);

    xUnitAssert &CustomMessage(const std::string &message);
    xUnitAssert &Expected(const std::string &expected);
    xUnitAssert &Actual(const std::string &actual);

    template<typename T>
    xUnitAssert &AppendUserMessage(const T &value)
    {
        std::stringstream str;
        str << value;
        userMessage.push_back(str.str());

        return *this;
    }

    const xUnitpp::LineInfo &LineInfo() const;

    static const xUnitAssert &None();

    virtual const char *what() const noexcept(true) override;

private:
    xUnitpp::LineInfo lineInfo;
    std::string call;
    std::string customMessage;
    std::string expected;
    std::string actual;
    std::vector<std::string> userMessage;

    mutable std::string whatMessage;
};

class xUnitFailure
{
private:
    xUnitFailure();

public:
    xUnitFailure(const xUnitAssert &assert, std::function<void(const xUnitAssert &)> onFailureComplete);
    xUnitFailure(const xUnitFailure &other);
    ~xUnitFailure() noexcept(false);

    static xUnitFailure None();

    template<typename T>
    xUnitFailure &operator <<(const T &value)
    {
        assert.AppendUserMessage(value);
        return *this;
    }

private:
    xUnitFailure &operator =(xUnitFailure other);

private:
    std::function<void(const xUnitAssert &)> OnFailureComplete;

    xUnitAssert assert;
    int &refCount;
};

class Assert
{
protected:
    static double round(double value, size_t precision);

    template<typename T>
    static std::string RangeToString(T &&begin, T &&end)
    {
        typedef decltype(*begin) val_type;

        std::string result = "[ ";

        std::for_each(std::forward<T>(begin), std::forward<T>(end), [&result](val_type &&val) { result += ToString(std::forward<val_type>(val)) + ", "; });

        result[result.size() - 2] = ' ';
        result[result.size() - 1] = ']';

        return result;
    }

    template<typename T>
    struct has_empty
    {
    private:
        template<typename U, U>
        class check {};

        template<typename C>
        static char f(check<bool (C::*)() const, &C::empty> *);

        template<typename C>
        static long f(...);

    public:
        static const bool value = (sizeof(f<T>(nullptr)) == sizeof(char));
    };

    xUnitFailure OnFailure(xUnitAssert assert) const;
    xUnitFailure OnSuccess() const;

    std::string callPrefix;
    std::function<void (const xUnitAssert &)> handleFailure;

public:
    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure Equal(TExpected expected, TActual actual, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (!comparer(expected, actual))
        {
            return OnFailure(xUnitAssert(callPrefix + "Equal", lineInfo).Expected(ToString(expected)).Actual(ToString(actual)));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    xUnitFailure Equal(TExpected expected, TActual actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return Equal(expected, actual, [](TExpected expected, TActual actual) { return expected == actual; }, lineInfo);
    }

    xUnitFailure Equal(const char *expected, const char *actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Equal(const char *expected, const std::string &actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Equal(const std::string &expected, const char *actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Equal(const std::string &expected, const std::string &actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Equal(float expected, float actual, int precision, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Equal(double expected, double actual, int precision, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure Equal(TExpected &&expectedBegin, TExpected &&expectedEnd, TActual &&actualBegin, TActual &&actualEnd, TComparer &&comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        auto expected = expectedBegin;
        auto actual = actualBegin;

        size_t index = 0;
        while (expected != expectedEnd && actual != actualEnd)
        {
            if (!comparer(*expected, *actual))
            {
                break;
            }

            ++expected;
            ++actual;
            ++index;
        }

        if (expected != expectedEnd || actual != actualEnd)
        {
            return OnFailure(xUnitAssert(callPrefix + "Equal", lineInfo)
                .CustomMessage("Sequence unequal at location " + ToString(index) + ".")
                .Expected(RangeToString(std::forward<TExpected>(expectedBegin), std::forward<TExpected>(expectedEnd)))
                .Actual(RangeToString(std::forward<TActual>(actualBegin), std::forward<TActual>(actualEnd))));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    xUnitFailure Equal(TExpected &&expectedBegin, TExpected &&expectedEnd, TActual &&actualBegin, TActual &&actualEnd, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return Equal(std::forward<TExpected>(expectedBegin), std::forward<TExpected>(expectedEnd), std::forward<TActual>(actualBegin), std::forward<TActual>(actualEnd), [](decltype(*expectedBegin) a, decltype(*actualBegin) b) { return a == b; }, lineInfo);
    }

    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure NotEqual(TExpected expected, TActual actual, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (comparer(expected, actual))
        {
            return OnFailure(xUnitAssert(callPrefix + "NotEqual", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    xUnitFailure NotEqual(TExpected expected, TActual actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return NotEqual(expected, actual, [](TExpected expected, TActual actual) { return expected == actual; }, lineInfo);
    }

    xUnitFailure NotEqual(const char *expected, const char *actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure NotEqual(const char *expected, const std::string &actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure NotEqual(const std::string &expected, const char *actual, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure NotEqual(const std::string &expected, const std::string &actual, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TExpected, typename TActual, typename TComparer>
    xUnitFailure NotEqual(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        auto expected = expectedBegin;
        auto actual = actualBegin;

        while (expected != expectedEnd && actual != actualEnd)
        {
            if (!comparer(*expected, *actual))
            {
                break;
            }

            ++expected;
            ++actual;
        }

        if (expected == expectedEnd && actual == actualEnd)
        {
            return OnFailure(xUnitAssert(callPrefix + "NotEqual", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TExpected, typename TActual>
    xUnitFailure NotEqual(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return NotEqual(expectedBegin, expectedEnd, actualBegin, actualEnd, [](decltype(*expectedBegin) a, decltype(*actualBegin) b) { return a == b; }, lineInfo);
    }


    template<typename TFunc>
    xUnitFailure DoesNotThrow(TFunc &&fn, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        try
        {
            fn();
        }
        catch (const std::exception &e)
        {
            return OnFailure(xUnitAssert(callPrefix + "DoesNotThrow", lineInfo)
                .Expected("(no exception)")
                .Actual(e.what()));
        }
        catch (...)
        {
            return OnFailure(xUnitAssert(callPrefix + "DoesNotThrow", lineInfo)
                .Expected("(no exception)")
                .Actual("Crash: unknown exception."));
        }

        return OnSuccess();
    }

    xUnitFailure Fail(const LineInfo &lineInfo = LineInfo::empty()) const;

    xUnitFailure False(bool b, const LineInfo &lineInfo = LineInfo::empty()) const;

    xUnitFailure True(bool b, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TSequence>
    typename std::enable_if<has_empty<TSequence>::value, xUnitFailure>::type Empty(const TSequence &sequence, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (!sequence.empty())
        {
            return OnFailure(xUnitAssert(callPrefix + "Empty", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<TSequence>::value, xUnitFailure>::type Empty(const TSequence &sequence, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;

        if (begin(sequence) != end(sequence))
        {
            return OnFailure(xUnitAssert(callPrefix + "Empty", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TSequence>
    typename std::enable_if<has_empty<TSequence>::value, xUnitFailure>::type NotEmpty(const TSequence &sequence, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (sequence.empty())
        {
            return OnFailure(xUnitAssert(callPrefix + "NotEmpty", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<TSequence>::value, xUnitFailure>::type NotEmpty(const TSequence &sequence, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;

        if (begin(sequence) == end(sequence))
        {
            return OnFailure(xUnitAssert(callPrefix + "NotEmpty", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TSequence, typename TPredicate>
    xUnitFailure DoesNotContainPred(const TSequence &sequence, TPredicate &&predicate, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        auto found = std::find_if(std::begin(sequence), std::end(sequence), std::forward<TPredicate>(predicate));
        if (found != std::end(sequence))
        {
            return OnFailure(xUnitAssert(callPrefix + "DoesNotContain", lineInfo)
                .CustomMessage("Found: matching value at position " + ToString(std::distance(std::begin(sequence), found)) + "."));
        }

        return OnSuccess();
    }

    template<typename TSequence, typename T>
    xUnitFailure DoesNotContain(const TSequence &sequence, T value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return DoesNotContainPred(sequence, [&value](const T& actual) { return actual == value; }, lineInfo);
    }

    xUnitFailure DoesNotContain(const char *actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure DoesNotContain(const char *actualString, const std::string &value, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure DoesNotContain(const std::string &actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure DoesNotContain(const std::string &actualString, const std::string &value, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TSequence, typename TPredicate>
    xUnitFailure ContainsPred(const TSequence &sequence, TPredicate &&predicate, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;

        if (find_if(begin(sequence), end(sequence), std::forward<TPredicate>(predicate)) == end(sequence))
        {
            return OnFailure(xUnitAssert(callPrefix + "Contains", lineInfo));
        }

        return OnSuccess();
    }

    template<typename TSequence, typename T>
    xUnitFailure Contains(const TSequence &sequence, T value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return ContainsPred(sequence, [&value](const T &actual) { return actual == value; }, lineInfo);
    }

    xUnitFailure Contains(const char *actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Contains(const char *actualString, const std::string &value, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Contains(const std::string &actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;
    xUnitFailure Contains(const std::string &actualString, const std::string &value, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TActual, typename TRange>
    xUnitFailure InRange(TActual actual, TRange min, TRange max, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.InRange argument error: min (" + ToString(min) + ") must be strictly less than max (" + ToString(max) + ").");
        }

        if (actual < min || actual >= max)
        {
            return OnFailure(xUnitAssert(callPrefix + "InRange", lineInfo)
                .Expected("[" + ToString(min) + " - " + ToString(max) + ")")
                .Actual(ToString(actual)));
        }

        return OnSuccess();
    }

    template<typename TActual, typename TRange>
    xUnitFailure NotInRange(TActual actual, TRange min, TRange max, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.NotInRange argument error: min (" + ToString(min) + ") must be strictly less than max (" + ToString(max) + ").");
        }

        if (actual >= min && actual < max)
        {
            return OnFailure(xUnitAssert(callPrefix + "NotInRange", lineInfo)
                .Expected("[" + ToString(min) + " - " + ToString(max) + ")")
                .Actual(ToString(actual)));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure NotNull(const T &value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (value == nullptr)
        {
            return OnFailure(xUnitAssert(callPrefix + "NotNull", lineInfo));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Null(const T &value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (value != nullptr)
        {
            return OnFailure(xUnitAssert(callPrefix + "Null", lineInfo));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure NotSame(const T &expected, const T &actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (&expected == &actual)
        {
            return OnFailure(xUnitAssert(callPrefix + "NotSame", lineInfo));
        }

        return OnSuccess();
    }


    template<typename T>
    xUnitFailure NotSame(const T *expected, const T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected == actual)
        {
            return OnFailure(xUnitAssert(callPrefix + "NotSame", lineInfo));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure NotSame(T *expected, T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected == actual)
        {
            return OnFailure(xUnitAssert(callPrefix + "NotSame", lineInfo));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Same(const T &expected, const T &actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (&expected != &actual)
        {
            return OnFailure(xUnitAssert(callPrefix + "Same", lineInfo));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Same(T *expected, T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected != actual)
        {
            return OnFailure(xUnitAssert("Same", lineInfo));
        }

        return OnSuccess();
    }

    template<typename T>
    xUnitFailure Same(const T *expected, const T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected != actual)
        {
            return OnFailure(xUnitAssert("Same", lineInfo));
        }

        return OnSuccess();
    }

    Assert(const std::string &callPrefix = "Assert.",
           std::function<void (const xUnitAssert &)> onFailure = [](const xUnitAssert &assert) { throw assert; });
};

const class : public Assert
{
public:
    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        try
        {
            fn();
        }
        catch (const TException &e)
        {
            return e;
        }
        catch (const std::exception &e)
        {
            throw xUnitAssert(callPrefix + "Throws", lineInfo)
                .Expected(typeid(TException).name())
                .Actual(e.what())
                .AppendUserMessage(msg);
        }
        catch (...)
        {
            throw xUnitAssert(callPrefix + "Throws", lineInfo)
                .Expected(typeid(TException).name())
                .Actual("Crash: unknown exception.")
                .AppendUserMessage(msg);
        }

        throw xUnitAssert(callPrefix + "Throws", lineInfo)
            .Expected(typeid(TException).name())
            .Actual("No exception.")
            .AppendUserMessage(msg);
    }

    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return Throws<TException>(std::forward<TFunc>(fn), "", lineInfo);
    }
} Assert;

}

#endif
