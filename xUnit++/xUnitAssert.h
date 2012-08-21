#ifndef XUNITASSERT_H_
#define XUNITASSERT_H_

#include <algorithm>
#include <exception>
#include <string>
#include <type_traits>

namespace xUnitpp
{

class xUnitAssert : public std::exception
{
    typedef std::exception base;

public:
    explicit xUnitAssert(const std::string &call, const std::string &userMsg, const std::string &customMsg,
                         const std::string &expected, const std::string &actual);
};

const class Assert
{
private:
    static double round(double value, size_t precision);

    template<typename T0>
    static std::string RangeToString(const T0 &begin, const T0 &end)
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
    template<typename T0, typename T1, typename TComparer>
    void Equal(T0 t0, T1 t1, TComparer comparer, const std::string &msg = "") const
    {
        using namespace std;

        if (!comparer(t0, t1))
        {
            throw xUnitAssert("Equal", msg, "", to_string(t0), to_string(t1));
        }
    }

    template<typename T0, typename T1>
    void Equal(T0 t0, T1 t1, const std::string &msg = "") const
    {
        Equal(t0, t1, [](T0 t0, T1 t1) { return t0 == t1; }, msg);
    }

    void Equal(float expected, float actual, int precision, const std::string &msg = "") const;
    void Equal(double expected, double actual, int precision, const std::string &msg = "") const;

    template<typename TSeq0, typename TSeq1, typename TComparer>
    void Equal(const TSeq0 &begin0, const TSeq0 &end0, const TSeq1 &begin1, const TSeq1 &end1, TComparer comparer, const std::string &msg = "") const
    {
        auto cur0 = begin0;
        auto cur1 = begin1;

        size_t index = 0;
        while (cur0 != end0 && cur1 != end1)
        {
            if (!comparer(*cur0, *cur1))
            {
                break;
            }

            ++cur0;
            ++cur1;
            ++index;
        }

        if (cur0 != end0 || cur1 != end1)
        {
            throw xUnitAssert("Equal", msg,
                "Sequence unequal at location " + std::to_string(index) + ".",
                RangeToString(begin0, end0),
                RangeToString(begin1, end1));
        }
    }

    template<typename TSeq0, typename TSeq1>
    void Equal(const TSeq0 &begin0, const TSeq0 &end0, const TSeq1 &begin1, const TSeq1 &end1, const std::string &msg = "") const
    {
        Equal(begin0, end0, begin1, end1, [](decltype(*begin0) a, decltype(*begin1) b) { return a == b; }, msg);
    }

    template<typename T0, typename T1, typename TComparer>
    void NotEqual(T0 t0, T1 t1, TComparer comparer, const std::string &msg = "") const
    {
        if (comparer(t0, t1))
        {
            throw xUnitAssert("NotEqual", msg, "", "", "");
        }
    }

    template<typename T0, typename T1>
    void NotEqual(T0 t0, T1 t1, const std::string &msg = "") const
    {
        NotEqual(t0, t1, [](T0 t0, T1 t1) { return t0 == t1; }, msg);
    }

    template<typename TSeq0, typename TSeq1, typename TComparer>
    void NotEqual(const TSeq0 &begin0, const TSeq0 &end0, const TSeq1 &begin1, const TSeq1 &end1, TComparer comparer, const std::string &msg = "") const
    {
        auto cur0 = begin0;
        auto cur1 = begin1;

        while (cur0 != end0 && cur1 != end1)
        {
            if (!comparer(*cur0, *cur1))
            {
                return;
            }

            ++cur0;
            ++cur1;
        }

        if (cur0 == end0 && cur1 == end1)
        {
            throw xUnitAssert("NotEqual", msg, "", "", "");
        }
    }

    template<typename TSeq0, typename TSeq1>
    void NotEqual(const TSeq0 &begin0, const TSeq0 &end0, const TSeq1 &begin1, const TSeq1 &end1, const std::string &msg = "") const
    {
        NotEqual(begin0, end0, begin1, end1, [](decltype(*begin0) a, decltype(*begin1) b) { return a == b; }, msg);
    }

    template<typename TFunc>
    void DoesNotThrow(TFunc &&fn, const std::string &msg = "") const
    {
        try
        {
            fn();
        }
        catch (std::exception &e)
        {
            throw xUnitAssert("DoesNotThrow", msg, "", "(no exception)", e.what());
        }
        catch (...)
        {
            throw xUnitAssert("DoesNotThrow", msg, "", "(no exception)", "Crash: unknown exception.");
        }
    }

    template<typename TException, typename TFunc>
    TException Throws(TFunc &&fn, const std::string &msg = "") const
    {
        try
        {
            fn();
        }
        catch (TException e)
        {
            return e;
        }
        catch (const std::exception &e)
        {
            throw xUnitAssert("Throws", msg, "", typeid(TException).name(), e.what());
        }
        catch (...)
        {
            throw xUnitAssert("Throws", msg, "", typeid(TException).name(), "Crash: unknown exception.");
        }

        throw xUnitAssert("Throws", msg, "", typeid(TException).name(), "No exception.");
    }

    void Fail(const std::string &msg = "") const;

    void False(bool b, const std::string &msg = "") const;

    void True(bool b, const std::string &msg = "") const;

    template<typename TSequence>
    typename std::enable_if<has_empty<TSequence>::value>::type Empty(const TSequence &sequence, const std::string &msg = "") const
    {
        if (!sequence.empty())
        {
            throw xUnitAssert("Empty", msg, "", "", "");
        }
    }

    template<typename TSequence>
    typename std::enable_if<!has_empty<TSequence>::value>::type Empty(const TSequence &sequence, const std::string &msg = "") const
    {
        using namespace std;
    
        if (begin(sequence) != end(sequence))
        {
            throw xUnitAssert("Empty", msg, "", "", "");
        }
    }

    template<typename TSequence, typename T>
    void DoesNotContain(const TSequence &sequence, T value, const std::string &msg = "") const
    {
        using namespace std;

        auto found = find(begin(sequence), end(sequence), value);
        if (found != end(sequence))
        {
            throw xUnitAssert("DoesNotContain", msg, "Found: " + to_string(value) + " at position " + to_string(distance(begin(sequence), found)) + ".", "", "");
        }
    }

    void DoesNotContain(const char *actualString, const char *value, const std::string &msg = "") const;

    void DoesNotContain(const std::string &actualString, const char *value, const std::string &msg = "") const;

    void DoesNotContain(const std::string &actualString, const std::string &value, const std::string &msg = "") const;

    template<typename TSequence, typename T>
    void Contains(const TSequence &sequence, T value, const std::string &msg = "") const
    {
        using namespace std;

        if (find(begin(sequence), end(sequence), value) == end(sequence))
        {
            throw xUnitAssert("Contains", msg, "", "", "");
        }
    }

    void Contains(const char *actualString, const char *value, const std::string &msg = "") const;

    void Contains(const std::string &actualString, const char *value, const std::string &msg = "") const;

    void Contains(const std::string &actualString, const std::string &value, const std::string &msg = "") const;

    template<typename TActual, typename TRange>
    void InRange(TActual actual, TRange min, TRange max, const std::string &msg = "") const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.InRange argument error: min (" + std::to_string(min) + ") must be strictly less than max (" + std::to_string(max) + ").");
        }

        if (actual < min || actual >= max)
        {
            throw xUnitAssert("InRange", msg, "", "[" + std::to_string(min) + " - " + std::to_string(max) + ")", std::to_string(actual));
        }
    }

    template<typename TActual, typename TRange>
    void NotInRange(TActual actual, TRange min, TRange max, const std::string &msg = "") const
    {
        if (min >= max)
        {
            throw std::invalid_argument("Assert.NotInRange argument error: min (" + std::to_string(min) + ") must be strictly less than max (" + std::to_string(max) + ").");
        }

        if (actual >= min && actual < max)
        {
            throw xUnitAssert("NotInRange", msg, "", "[" + std::to_string(min) + " - " + std::to_string(max) + ")", std::to_string(actual));
        }
    }

    template<typename T>
    void NotNull(const T &value, const std::string &msg = "") const
    {
        if (value == nullptr)
        {
            throw xUnitAssert("NotNull", msg, "", "", "");
        }
    }

    template<typename T>
    void Null(const T &value, const std::string &msg = "") const
    {
        if (value != nullptr)
        {
            throw xUnitAssert("Null", msg, "", "", "");
        }
    }

    template<typename T>
    void NotSame(const T &t0, const T &t1, const std::string &msg = "") const
    {
        if (&t0 == &t1)
        {
            throw xUnitAssert("NotSame", msg, "", "", "");
        }
    }

    template<typename T>
    void NotSame(const T *t0, const T *t1, const std::string &msg = "") const
    {
        if (t0 == t1)
        {
            throw xUnitAssert("NotSame", msg, "", "", "");
        }
    }

    template<typename T>
    void Same(const T &t0, const T &t1, const std::string &msg = "") const
    {
        if (&t0 != &t1)
        {
            throw xUnitAssert("Same", msg, "", "", "");
        }
    }

    template<typename T>
    void Same(T *t0, T *t1, const std::string &msg = "") const
    {
        if (t0 != t1)
        {
            throw xUnitAssert("Same", msg, "", "", "");
        }
    }

    template<typename T>
    void Same(const T *t0, const T *t1, const std::string &msg = "") const
    {
        if (t0 != t1)
        {
            throw xUnitAssert("Same", msg, "", "", "");
        }
    }
} Assert;

}

#endif
