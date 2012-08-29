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
{
}

TestDetails::TestDetails(const std::string &name, const std::string &suite, const AttributeCollection &attributes,
                         xUnitpp::Duration timeLimit, const std::string &filename, int line)
    : Id(NextId())
    , Name(name)
    , Suite(suite)
    , Attributes(attributes)
    , TimeLimit(timeLimit)
    , Filename(filename)
    , Line(line)
{
}

TestDetails::TestDetails(const TestDetails &other)
    : Id(other.Id)
    , Name(other.Name)
    , Suite(other.Suite)
    , Attributes(other.Attributes)
    , TimeLimit(other.TimeLimit)
    , Filename(other.Filename)
    , Line(other.Line)
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
    swap(td0.Suite, td1.Suite);
    swap(td0.Attributes, td1.Attributes);
    swap(td0.TimeLimit, td1.TimeLimit);
    swap(td0.Filename, td1.Filename);
    swap(td0.Line, td1.Line);
}

}
