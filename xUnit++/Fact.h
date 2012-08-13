#ifndef FACT_H_
#define FACT_H_

#include <chrono>
#include <functional>
#include <string>
#include <vector>
#include "TestDetails.h"

namespace xUnitpp
{

class Fact
{
public:
    Fact(std::function<void()> fact, const std::string &name, const std::string &suite, std::chrono::milliseconds timeLimit, const std::string &filename, int line);
    Fact(const Fact &other);
    Fact(Fact &&other);
    Fact &operator =(Fact other);
    friend void swap(Fact &f0, Fact &f1);

    const TestDetails &TestDetails() const;

    std::function<void()> Test() const;

private:
    std::function<void()> mFact;

    xUnitpp::TestDetails mTestDetails;
};

}

#endif
