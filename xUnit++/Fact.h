#ifndef FACT_H_
#define FACT_H_

#include <functional>
#include <string>
#include <vector>
#include "TestDetails.h"

namespace xUnitpp
{

class Fact
{
public:
    Fact(std::function<void()> fact, const std::string &name, const std::string &suite, const std::string &filename, int line);
    Fact(const Fact &other);
    Fact(Fact &&other);
    Fact &operator =(Fact other);
    friend void swap(Fact &f0, Fact &f1);

    const std::string &Suite() const;

    void Run();

private:
    std::function<void()> mFact;

    TestDetails mTestDetails;
};

}

#endif
