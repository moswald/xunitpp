#include "Theory.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

Theory::Theory(const std::vector<std::function<void()>> &theorySet, const std::string &name, const std::string &suite, const std::string &filename, int line)
    : mTheories(theorySet)
    , mTestDetails(name, suite, filename, line)
{
}

Theory::Theory(const Theory &other)
    : mTheories(other.mTheories)
    , mTestDetails(other.mTestDetails)
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
    swap(f0.mTestDetails, f1.mTestDetails);
}

const std::string &Theory::Suite() const
{
    return mTestDetails.Suite;
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
