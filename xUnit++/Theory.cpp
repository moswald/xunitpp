#include "Theory.h"
#include "xUnitAssert.h"

namespace xUnitpp
{

Theory::Theory(const std::vector<std::function<void()>> &theorySet, const std::string &name, const std::string &suite, std::chrono::milliseconds timeLimit, const std::string &filename, int line)
    : mTheories(theorySet)
    , mTestDetails(name, suite, timeLimit, filename, line)
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

const TestDetails &Theory::TestDetails() const
{
    return mTestDetails;
}

const std::vector<std::function<void()>> &Theory::Theories() const
{
    return mTheories;
}

    }
