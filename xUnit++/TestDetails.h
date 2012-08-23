#ifndef TESTDETAILS_H_
#define TESTDETAILS_H_

#include <chrono>
#include <map>
#include <string>
#include <tuple>

namespace xUnitpp
{

typedef std::map<std::string, std::string> AttributeCollection;

struct TestDetails
{
    TestDetails();
    TestDetails(const std::string &name, const std::string &suite,
        const AttributeCollection &attributes,
        std::chrono::milliseconds timeLimit, const std::string &filename, int line);
    TestDetails(const TestDetails &other);
    TestDetails(TestDetails &&other);
    TestDetails &operator =(TestDetails other);
    friend void swap(TestDetails &td0, TestDetails &td1);

    std::string Name;
    std::string Suite;
    AttributeCollection Attributes;
    std::chrono::milliseconds TimeLimit;
    std::string Filename;
    int Line;
};

}

#endif
