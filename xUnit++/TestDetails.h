#ifndef TESTDETAILS_H_
#define TESTDETAILS_H_

#include <chrono>
#include <string>

namespace xUnitpp
{

struct TestDetails
{
    TestDetails();
    TestDetails(const std::string &name, const std::string &suite, std::chrono::milliseconds timeLimit, const std::string &filename, int line);
    TestDetails(const TestDetails &other);
    TestDetails(TestDetails &&other);
    TestDetails &operator =(TestDetails other);
    friend void swap(TestDetails &td0, TestDetails &td1);

    std::string Name;
    std::string Suite;
    std::chrono::milliseconds TimeLimit;
    std::string Filename;
    int Line;
};

}

#endif
