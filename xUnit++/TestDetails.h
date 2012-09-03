#ifndef TESTDETAILS_H_
#define TESTDETAILS_H_

#include <chrono>
#include <map>
#include <string>
#include <tuple>
#include "xUnitTime.h"

namespace xUnitpp
{

typedef std::multimap<std::string, std::string> AttributeCollection;

struct TestDetails
{
    TestDetails();
    TestDetails(const std::string &name, const std::string &suite,
        const AttributeCollection &attributes, Time::Duration timeLimit,
        const std::string &filename, int line);
    TestDetails(const TestDetails &other);
    TestDetails(TestDetails &&other);
    TestDetails &operator =(TestDetails other);
    friend void swap(TestDetails &td0, TestDetails &td1);

    int Id;
    std::string Name;
    std::string Suite;
    AttributeCollection Attributes;
    Time::Duration TimeLimit;
    std::string Filename;
    int Line;
};

}

#endif
