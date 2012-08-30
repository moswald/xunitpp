#ifndef XUNITASSERT_H_
#define XUNITASSERT_H_

#include <algorithm>
#include <exception>
#include <string>
#include <type_traits>
#include "LineInfo.h"

namespace xUnitpp
{

class xUnitAssert : public std::exception
{
    typedef std::exception base;

public:
    xUnitAssert(const std::string &call, const std::string &userMsg, const std::string &customMsg,
                const std::string &expected, const std::string &actual, const LineInfo &lineInfo);
    xUnitAssert(const xUnitAssert &other);

    const LineInfo &LineInfo() const;

private:
    xUnitpp::LineInfo lineInfo;
};

const class Assert
{
private:
    static double round(double value, size_t precision);

    template<typename T>
    static std::string RangeToString(const T &begin, const T &end)
    {
        std::string result = "[ ";

        std::for_each(begin, end, [&result](decltype(*begin) val) { result += std::to_string(val) + ", "; });

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

public:
    template<typename TExpected, typename TActual, typename TComparer>
    void Equal(TExpected expected, TActual actual, TComparer comparer, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;

        if (!comparer(expected, actual))
        {
            throw xUnitAssert("Equal", msg, "", to_string(expected), to_string(actual), lineInfo);
        }
    }

    template<typename TExpected, typename TActual, typename TComparer>
    void Equal(TExpected expected, TActual actual, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Equal(expected, actual, comparer, "", lineInfo);
    }

    template<typename TExpected, typename TActual>
    void Equal(TExpected expected, TActual actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Equal(expected, actual, [](TExpected expected, TActual actual) { return expected == actual; }, msg, lineInfo);
    }

    template<typename TExpected, typename TActual>
    void Equal(TExpected expected, TActual actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Equal(expected, actual, std::string(""), lineInfo);
    }

    void Equal(const char *expected, const char *actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Equal(const char *expected, const char *actual, const LineInfo &lineInfo = LineInfo::empty()) const;

    void Equal(const std::string &expected, const char *actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Equal(const std::string &expected, const char *actual, const LineInfo &lineInfo = LineInfo::empty()) const;

    void Equal(const std::string &expected, const std::string &actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Equal(const std::string &expected, const std::string &actual, const LineInfo &lineInfo = LineInfo::empty()) const;

    void Equal(float expected, float actual, int precision, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Equal(float expected, float actual, int precision, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Equal(double expected, double actual, int precision, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Equal(double expected, double actual, int precision, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TExpected, typename TActual, typename TComparer>
    void Equal(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, TComparer comparer, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
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
            throw xUnitAssert("Equal", msg,
                "Sequence unequal at location " + std::to_string(index) + ".",
                RangeToString(expectedBegin, expectedEnd),
                RangeToString(actualBegin, actualEnd),
                lineInfo);
        }
    }

    template<typename TExpected, typename TActual, typename TComparer>
    void Equal(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Equal(expectedBegin, expectedEnd, actualBegin, actualEnd, comparer, "", lineInfo);
    }

    template<typename TExpected, typename TActual>
    void Equal(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Equal(expectedBegin, expectedEnd, actualBegin, actualEnd, [](decltype(*expectedBegin) a, decltype(*actualBegin) b) { return a == b; }, msg, lineInfo);
    }

    template<typename TExpected, typename TActual>
    void Equal(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Equal(expectedBegin, expectedEnd, actualBegin, actualEnd, std::string(""), lineInfo);
    }

    template<typename TExpected, typename TActual, typename TComparer>
    void NotEqual(TExpected expected, TActual actual, TComparer comparer, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (comparer(expected, actual))
        {
            throw xUnitAssert("NotEqual", msg, "", "", "", lineInfo);
        }
    }

    template<typename TExpected, typename TActual, typename TComparer>
    void NotEqual(TExpected expected, TActual actual, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotEqual(expected, actual, comparer, "", lineInfo);
    }

    template<typename TExpected, typename TActual>
    void NotEqual(TExpected expected, TActual actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotEqual(expected, actual, [](TExpected expected, TActual actual) { return expected == actual; }, msg, lineInfo);
    }

    template<typename TExpected, typename TActual>
    void NotEqual(TExpected expected, TActual actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotEqual(expected, actual, std::string(""), lineInfo);
    }

    template<typename TExpected, typename TActual, typename TComparer>
    void NotEqual(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, TComparer comparer, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        auto expected = expectedBegin;
        auto actual = actualBegin;

        while (expected != expectedEnd && actual != actualEnd)
        {
            if (!comparer(*expected, *actual))
            {
                return;
            }

            ++expected;
            ++actual;
        }

        if (expected == expectedEnd && actual == actualEnd)
        {
            throw xUnitAssert("NotEqual", msg, "", "", "", lineInfo);
        }
    }

    template<typename TExpected, typename TActual, typename TComparer>
    void NotEqual(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, TComparer comparer, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotEqual(expectedBegin, expectedEnd, actualBegin, actualEnd, comparer, "", lineInfo);
    }

    template<typename TExpected, typename TActual>
    void NotEqual(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotEqual(expectedBegin, expectedEnd, actualBegin, actualEnd, [](decltype(*expectedBegin) a, decltype(*actualBegin) b) { return a == b; }, msg, lineInfo);
    }

    template<typename TExpected, typename TActual>
    void NotEqual(const TExpected &expectedBegin, const TExpected &expectedEnd, const TActual &actualBegin, const TActual &actualEnd, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotEqual(expectedBegin, expectedEnd, actualBegin, actualEnd, std::string(""), lineInfo);
    }

    template<typename TFunc>
    void DoesNotThrow(TFunc &&fn, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        try
        {
            fn();
        }
        catch (const std::exception &e)
        {
            throw xUnitAssert("DoesNotThrow", msg, "", "(no exception)", e.what(), lineInfo);
        }
        catch (...)
        {
            throw xUnitAssert("DoesNotThrow", msg, "", "(no exception)", "Crash: unknown exception.", lineInfo);
        }
    }

    template<typename TFunc>
    void DoesNotThrow(TFunc &&fn, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        DoesNotThrow(fn, "", lineInfo);
    }

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
            throw xUnitAssert("Throws", msg, "", typeid(TException).name(), e.what(), lineInfo);
        }
        catch (...)
        {
            throw xUnitAssert("Throws", msg, "", typeid(TException).name(), "Crash: unknown exception.", lineInfo);
        }

        throw xUnitAssert("Throws", msg, "", typeid(TException).name(), "No exception.", lineInfo);
    }

    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        return Throws<TException>(fn, "", lineInfo);
    }

    void Fail(const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Fail(const LineInfo &lineInfo = LineInfo::empty()) const;

    void False(bool b, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void False(bool b, const LineInfo &lineInfo = LineInfo::empty()) const;

    void True(bool b, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void True(bool b, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TSequence>
    typename std::enable_if<has_empty<TSequence>::value>::type Empty(const TSequence &sequence, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (!sequence.empty())
        {
            throw xUnitAssert("Empty", msg, "", "", "", lineInfo);
        }
    }

    template<typename TSequence>
    typename std::enable_if<has_empty<TSequence>::value>::type Empty(const TSequence &sequence, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Empty(sequence, "", lineInfo);
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<TSequence>::value>::type Empty(const TSequence &sequence, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;
    
        if (begin(sequence) != end(sequence))
        {
            throw xUnitAssert("Empty", msg, "", "", "", lineInfo);
        }
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<TSequence>::value>::type Empty(const TSequence &sequence, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Empty(sequence, "", lineInfo);
    }

    template<typename TSequence, typename TPredicate>
    void DoesNotContainPred(const TSequence &sequence, TPredicate &&predicate, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;

        auto found = find_if(begin(sequence), end(sequence), predicate);
        if (found != end(sequence))
        {
            throw xUnitAssert("DoesNotContain", msg, "Found: matching value at position " + to_string(distance(begin(sequence), found)) + ".", "", "", lineInfo);
        }
    }

    template<typename TSequence, typename TPredicate>
    void DoesNotContainPred(const TSequence &sequence, TPredicate &&predicate, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        DoesNotContainPred(sequence, predicate, "", lineInfo);
    }

    template<typename TSequence, typename T>
    void DoesNotContain(const TSequence &sequence, T value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        DoesNotContainPred(sequence, [&value](const T& actual) { return actual == value; }, msg, lineInfo);
    }

    template<typename TSequence, typename T>
    void DoesNotContain(const TSequence &sequence, T value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        DoesNotContain(sequence, value, "", lineInfo);
    }

    void DoesNotContain(const char *actualString, const char *value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void DoesNotContain(const char *actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;

    void DoesNotContain(const std::string &actualString, const char *value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void DoesNotContain(const std::string &actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;

    void DoesNotContain(const std::string &actualString, const std::string &value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void DoesNotContain(const std::string &actualString, const std::string &value, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TSequence, typename TPredicate>
    void ContainsPred(const TSequence &sequence, TPredicate &&predicate, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        using namespace std;

        if (find_if(begin(sequence), end(sequence), predicate) == end(sequence))
        {
            throw xUnitAssert("Contains", msg, "", "", "", lineInfo);
        }
    }

    template<typename TSequence, typename TPredicate>
    void ContainsPred(const TSequence &sequence, TPredicate &&predicate, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        ContainsPred(sequence, predicate, "", lineInfo);
    }

    template<typename TSequence, typename T>
    void Contains(const TSequence &sequence, T value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        ContainsPred(sequence, [&value](const T &actual) { return actual == value; }, msg, lineInfo); 
    }

    template<typename TSequence, typename T>
    void Contains(const TSequence &sequence, T value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Contains(sequence, value, "", lineInfo);
    }

    void Contains(const char *actualString, const char *value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Contains(const char *actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;

    void Contains(const std::string &actualString, const char *value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Contains(const std::string &actualString, const char *value, const LineInfo &lineInfo = LineInfo::empty()) const;

    void Contains(const std::string &actualString, const std::string &value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const;
    void Contains(const std::string &actualString, const std::string &value, const LineInfo &lineInfo = LineInfo::empty()) const;

    template<typename TActual, typename TRange>
    void InRange(TActual actual, TRange min, TRange max, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.InRange argument error: min (" + std::to_string(min) + ") must be strictly less than max (" + std::to_string(max) + ").");
        }

        if (actual < min || actual >= max)
        {
            throw xUnitAssert("InRange", msg, "", "[" + std::to_string(min) + " - " + std::to_string(max) + ")", std::to_string(actual), lineInfo);
        }
    }

    template<typename TActual, typename TRange>
    void InRange(TActual actual, TRange min, TRange max, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        InRange(actual, min, max, "", lineInfo);
    }

    template<typename TActual, typename TRange>
    void NotInRange(TActual actual, TRange min, TRange max, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.NotInRange argument error: min (" + std::to_string(min) + ") must be strictly less than max (" + std::to_string(max) + ").");
        }

        if (actual >= min && actual < max)
        {
            throw xUnitAssert("NotInRange", msg, "", "[" + std::to_string(min) + " - " + std::to_string(max) + ")", std::to_string(actual), lineInfo);
        }
    }

    template<typename TActual, typename TRange>
    void NotInRange(TActual actual, TRange min, TRange max, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotInRange(actual, min, max, "", lineInfo);
    }

    template<typename T>
    void NotNull(const T &value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (value == nullptr)
        {
            throw xUnitAssert("NotNull", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void NotNull(const T &value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotNull(value, "", lineInfo);
    }

    template<typename T>
    void Null(const T &value, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (value != nullptr)
        {
            throw xUnitAssert("Null", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void Null(const T &value, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Null(value, "", lineInfo);
    }

    template<typename T>
    void NotSame(const T &expected, const T &actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (&expected == &actual)
        {
            throw xUnitAssert("NotSame", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void NotSame(const T &expected, const T &actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotSame(expected, actual, "", lineInfo);
    }

    template<typename T>
    void NotSame(const T *expected, const T *actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected == actual)
        {
            throw xUnitAssert("NotSame", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void NotSame(T *expected, T *actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected == actual)
        {
            throw xUnitAssert("NotSame", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void NotSame(T *expected, T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotSame(expected, actual, "", lineInfo);
    }

    template<typename T>
    void NotSame(const T *expected, const T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        NotSame(expected, actual, "", lineInfo);
    }

    template<typename T>
    void Same(const T &expected, const T &actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (&expected != &actual)
        {
            throw xUnitAssert("Same", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void Same(const T &expected, const T &actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Same(expected, actual, "", lineInfo);
    }

    template<typename T>
    void Same(T *expected, T *actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected != actual)
        {
            throw xUnitAssert("Same", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void Same(T *expected, T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Same(expected, actual, "", lineInfo);
    }

    template<typename T>
    void Same(const T *expected, const T *actual, const std::string &msg, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        if (expected != actual)
        {
            throw xUnitAssert("Same", msg, "", "", "", lineInfo);
        }
    }

    template<typename T>
    void Same(const T *expected, const T *actual, const LineInfo &lineInfo = LineInfo::empty()) const
    {
        Same(expected, actual, "", lineInfo);
    }
} Assert;

}

#endif
