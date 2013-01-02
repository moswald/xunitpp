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

    std::string GetFullName(const std::string &name, int testInstance, const std::string &params)
    {
        if (params.empty())
        {
            return name;
        }

        return name + "[" + std::to_string(testInstance) + "]" + params;
    }
}

namespace xUnitpp
{

TestDetails::TestDetails()
    : LineInfo()
{
}

TestDetails::TestDetails(std::string &&name, int testInstance, std::string &&params, const std::string &suite, AttributeCollection &&attributes,
                         Time::Duration timeLimit, std::string &&filename, int line)
    : Id(NextId())
    , TestInstance(testInstance)
    , Name(std::move(name))
    , Params(std::move(params))
    , FullName(::GetFullName(Name, testInstance, Params))
    , Suite(suite)
    , Attributes(std::move(attributes))
    , TimeLimit(timeLimit)
    , LineInfo(std::move(filename), line)
{
}

int __stdcall TestDetails::GetId() const
{
    return Id;
}

const char * __stdcall TestDetails::GetName() const 
{
    return Name.c_str();
}

const char * __stdcall TestDetails::GetFullName() const 
{
    return FullName.c_str();
}

const char * __stdcall TestDetails::GetSuite() const 
{
    return Suite.c_str();
}

const char * __stdcall TestDetails::GetParams() const
{
    return Params.c_str();
}

int __stdcall TestDetails::GetTestInstance() const
{
    return TestInstance;
}

size_t __stdcall TestDetails::GetAttributeCount() const 
{
    return Attributes.size();
}

const char * __stdcall TestDetails::GetAttributeKey(size_t index) const 
{
    return std::get<0>(Attributes[index]).c_str();
}

const char * __stdcall TestDetails::GetAttributeValue(size_t index) const 
{
    return std::get<1>(Attributes[index]).c_str();
}

void __stdcall TestDetails::FindAttributeKey(const char *key, size_t &begin, size_t &end) const
{
    auto range = Attributes.find(AttributeCollection::Attribute(key, ""));

    begin = std::distance(Attributes.begin(), range.first);
    end = std::distance(Attributes.end(), range.second);
}

const char * __stdcall TestDetails::GetFile() const 
{
    return LineInfo.file.c_str();
}

int __stdcall TestDetails::GetLine() const 
{
    return LineInfo.line;
}


}
