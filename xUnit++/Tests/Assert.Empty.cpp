#include <vector>
#include "../xUnit++.h"

using xUnitpp::xUnitAssert;
using xUnitpp::Assert;

namespace
{
    struct
    {
        bool empty() const
        {
            return true;
        }
    } hasConstEmpty;

    namespace Local
    {
        struct Container
        {
        };

        int begin(const Container &)
        {
            return 0;
        }

        int end(const Container &)
        {
            return 0;
        }
    }
}

SUITE(AssertEqual)
{

FACT(EmptyCallsEmptyForContainerWithConstEmptyMember)
{
    Assert.Empty(hasConstEmpty);
}

FACT(EmptyFindsLocalBeginEnd)
{
    Assert.Empty(Local::Container());
}

FACT(EmptyFindsStdBeginEnd)
{
    int array[10];
    Assert.Throws<xUnitAssert>([&]() { Assert.Empty(array); });
}

FACT(EmptyFindsMemberBeginEnd)
{
    struct
    {
        int begin() const
        {
            return 0;
        }

        int end() const
        {
            return 0;
        }
    } hasMembers;

    Assert.Empty(hasMembers);
}

FACT(EmptyAssertsOnFailure)
{
    std::vector<int> v;
    v.push_back(0);
    v.push_back(1);

    Assert.Throws<xUnitAssert>([&]() { Assert.Empty(v); });
}

FACT(EmptyAppendsUserMessage)
{
    static const std::string msg = "xUnit++";

    std::vector<int> v;
    v.push_back(0);
    v.push_back(1);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Empty(v, msg); });

    Assert.Contains(assert.what(), msg.c_str());
}

}