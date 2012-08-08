#include "Theory.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

Theory::Theory(const std::vector<std::function<void()>> &theorySet, const std::string &name, const std::string &suite, const std::string &filename, int line)
    : mTheories(theorySet)
    , mName(name)
    , mSuite(suite)
    , mFilename(filename)
    , mLine(line)
{
}

Theory::Theory(const Theory &other)
    : mTheories(other.mTheories)
    , mName(other.mName)
    , mSuite(other.mSuite)
    , mFilename(other.mFilename)
    , mLine(other.mLine)
{
}

Theory::Theory(Theory &&other)
{
    swap(*this, other);
}

Theory &Theory::operator =(Theory other)
{
    swap(*this, other);
    return *this;
}

void swap(Theory &f0, Theory &f1)
{
    using std::swap;

    swap(f0.mTheories, f1.mTheories);
    swap(f0.mName, f1.mName);
    swap(f0.mSuite, f1.mSuite);
    swap(f0.mFilename, f1.mFilename);
    swap(f0.mLine, f1.mLine);
}

const std::vector<std::function<void()>> &Theory::Theories() const
{
    return mTheories;
}

void Theory::Run(std::function<void()> theory)
{
    try
    {
        theory();
    }
    catch (xUnitAssert &)
    {
    }
    catch (std::exception &)
    {
    }
    catch (...)
    {
    }
}

}
