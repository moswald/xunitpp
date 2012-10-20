#include "TestDetails.h"
#include <utility>
#include "xUnitTime.h"

namespace
{
    inline int NextId()
    {
        static int id = 0;
        return id++;
    }
}

namespace xUnitpp
{

TestDetails::TestDetails()
    : LineInfo()
{
}

TestDetails::TestDetails(std::string &&name, std::string &&shortName, const std::string &suite, AttributeCollection &&attributes,
                         Time::Duration timeLimit, std::string &&filename, int line)
    : Id(NextId())
    , Name(std::move(name))
    , ShortName(std::move(shortName))
    , Suite(suite)
    , Attributes(std::move(attributes))
    , TimeLimit(timeLimit)
    , LineInfo(std::move(filename), line)
{
}

TestDetails::TestDetails(const TestDetails &other)
    : Id(other.Id)
    , Name(other.Name)
    , ShortName(other.ShortName)
    , Suite(other.Suite)
    , Attributes(other.Attributes)
    , TimeLimit(other.TimeLimit)
    , LineInfo(other.LineInfo)
{
}

TestDetails::TestDetails(TestDetails &&other)
{
    swap(*this, other);
}

TestDetails &TestDetails::operator =(TestDetails other)
{
    swap(*this, other);
    return *this;
}

void swap(TestDetails &td0, TestDetails &td1)
{
    using std::swap;

    swap(td0.Id, td1.Id);
    swap(td0.Name, td1.Name);
    swap(td0.ShortName, td1.ShortName);
    swap(td0.Suite, td1.Suite);
    swap(td0.Attributes, td1.Attributes);
    swap(td0.TimeLimit, td1.TimeLimit);
    swap(td0.LineInfo, td1.LineInfo);
}

}
