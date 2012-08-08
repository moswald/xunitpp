#include "Fact.h"
#include <utility>
#include "xUnitAssert.h"

namespace xUnitpp
{

Fact::Fact(std::function<void()> fact, const std::string &name, const std::string &suite, const std::string &filename, int line)
    : mFact(fact)
    , mName(name)
    , mSuite(suite)
    , mFilename(filename)
    , mLine(line)
{
}

Fact::Fact(const Fact &other)
    : mFact(other.mFact)
    , mName(other.mName)
    , mSuite(other.mSuite)
    , mFilename(other.mFilename)
    , mLine(other.mLine)
{
}

Fact::Fact(Fact &&other)
{
    swap(*this, other);
}

Fact &Fact::operator =(Fact other)
{
    swap(*this, other);
    return *this;
}

void swap(Fact &f0, Fact &f1)
{
    using std::swap;

    swap(f0.mFact, f1.mFact);
    swap(f0.mName, f1.mName);
    swap(f0.mSuite, f1.mSuite);
    swap(f0.mFilename, f1.mFilename);
    swap(f0.mLine, f1.mLine);
}

void Fact::Run()
{
    try
    {
        mFact();
    }
    catch (xUnitAssert &e)
    {
    }
    catch (std::exception &e)
    {
    }
    catch (...)
    {
    }
}

}
