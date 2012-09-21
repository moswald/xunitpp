#include <string>
#include <vector>
#include "xUnit++/xUnit++.h"

using xUnitpp::xUnitAssert;

SUITE("AssertDoesNotContain")
{

FACT("DoesNotContainForSequenceSuccess")
{
    std::vector<int> v;

    Assert.DoesNotContain(v, 0);
}

FACT("DoesNotContainForSequenceAssertsOnFailure")
{
    std::vector<int> v(1, 0);

    Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(v, 0); });
}

namespace TestNs_24
{
    using xUnitpp::Assert;
    std::shared_ptr<xUnitpp::Check> pCheck = std::make_shared<xUnitpp::Check>(); 
    class TestFixture_24 : public xUnitpp::NoFixture 
    {
        TestFixture_24 &operator =(TestFixture_24);
    public:
        TestFixture_24() : Check(*pCheck) { }
        void TestFn_24();
        
        const xUnitpp::Check &Check;
    };
    void TestRunner_24()
    {
        TestFixture_24().TestFn_24();
    }
    xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), &TestRunner_24, "ContainsForSequenceAppendsUserMessage", xUnitSuite::Name(), xUnitAttributes::Attributes(), -1, "assert.contains.cpp", 24, pCheck);
}
void TestNs_24 :: TestFixture_24 :: TestFn_24()
{
    static const std::string msg = "xUnit++";
    std::vector<int> v;

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.Contains(v, 0) << msg; });
    auto what = std::string(assert.what());

    Assert.NotEqual(std::string::npos, what.find(msg));
}

FACT("DoesNotContainForSequenceAppendsUserMessage")
{
    static const std::string msg = "xUnit++";
    std::vector<int> v(1, 0);

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(v, 0) << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

FACT("DoesNotContainForStringSuccess")
{
    std::string actual = "abcd";

    Assert.DoesNotContain(actual, "xyz");
}

FACT("DoesNotContainForStringAssertsOnFailure")
{
    std::string actual = "abcd";

    Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(actual, "bc"); });
}

FACT("DoesNotContainForStringAppendsUserMessage")
{
    static const std::string msg = "xUnit++";
    std::string actual = "abcd";

    auto assert = Assert.Throws<xUnitAssert>([&]() { Assert.DoesNotContain(actual, "ab") << msg; });

    Assert.Contains(assert.what(), msg.c_str());
}

}