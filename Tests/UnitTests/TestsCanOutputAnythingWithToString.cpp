#include <string>
#include "xUnit++/xUnit++.h"

SUITE("ADLToString")
{

namespace HasToString
{
    struct Point
    {
        Point()
            : x(0)
            , y(0)
        {
        }

        Point(int x, int y)
            : x(x)
            , y(y)
        {
        }

        friend bool operator <(const Point &p0, const Point &p1)
        {
            return p0.x < p1.x;
        }

        friend bool operator >=(const Point &p0, const Point &p1)
        {
            return p0.x >= p1.x;
        }

        friend bool operator ==(const Point &p0, const Point &p1)
        {
            return p0.x == p1.x && p0.y == p1.y;
        }

        friend std::string to_string(const Point &pt)
        {
            return "(" + std::to_string(pt.x) + ", " + std::to_string(pt.y) + ")";
        }

        int x;
        int y;
    };
}

using HasToString::Point;

FACT("Testing Equal with ADL")
{
    Point p0(0, 0);
    Point p1(1, 1);

    auto result = Assert.Throws<xUnitpp::xUnitAssert>([&]() { Assert.Equal(p0, p1); });

    Check.Contains(result.Expected(), "(0, 0)");
    Check.Contains(result.Actual(), "(1, 1)");
}

FACT("Testing range Equal with ADL")
{
    Point p0[] = { Point(0, 0), Point(1, 0), Point(2, 0) };
    Point p1[] = { Point(0, 0), Point(1, 1), Point(2, 0) };

    auto result = Assert.Throws<xUnitpp::xUnitAssert>([&]() { Assert.Equal(&p0[0], &p0[3], &p1[0], &p1[3]); });

    Check.Contains(result.CustomMessage(), "Sequence unequal at location 1.");
    Check.Contains(result.Expected(), "[ (0, 0), (1, 0), (2, 0) ]");
    Check.Contains(result.Actual(), "[ (0, 0), (1, 1), (2, 0) ]");
}

FACT("Testing InRange with ADL")
{
    Point min(0, 0);
    Point max(10, 0);
    Point test(20, 0);

    auto result = Assert.Throws<xUnitpp::xUnitAssert>([&]() { Assert.InRange(test, min, max); });

    Check.Contains(result.Expected(), "[(0, 0) - (10, 0))");
    Check.Contains(result.Actual(), "(20, 0)");
}

FACT("Testing NotInRange with ADL")
{
    Point min(0, 0);
    Point max(10, 0);
    Point test(5, 0);

    auto result = Assert.Throws<xUnitpp::xUnitAssert>([&]() { Assert.NotInRange(test, min, max); });

    Check.Contains(result.Expected(), "[(0, 0) - (10, 0))");
    Check.Contains(result.Actual(), "(5, 0)");
}

FACT("Testing overloaded << operator")
{
    // this will be a compile error if it is not supported
    Check.True(true) << Point(0, 0);
}

}
