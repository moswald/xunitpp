#ifndef THEORY_H_
#define THEORY_H_

#include <functional>
#include <string>
#include <vector>

namespace xUnitpp
{

class Theory
{
public:
    Theory(const std::vector<std::function<void()>> &theorySet, const std::string &name, const std::string &suite, const std::string &filename, int line);
    Theory(const Theory &other);
    Theory(Theory &&other);
    Theory &operator =(Theory other);
    friend void swap(Theory &f0, Theory &f1);

    void Run(std::function<void()> theory);

    const std::vector<std::function<void()>> &Theories() const;

private:
    std::vector<std::function<void()>> mTheories;

    std::string mName;
    std::string mSuite;
    std::string mFilename;
    int mLine;
};

}

#endif
