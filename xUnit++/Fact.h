#ifndef FACT_H_
#define FACT_H_

#include <functional>
#include <string>
#include <vector>

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

    void Run();

private:
    std::function<void()> mFact;

    std::string mName;
    std::string mSuite;
    std::string mFilename;
    int mLine;
};

}

#endif
