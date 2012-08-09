#include "Fact.h"
#include <utility>
#include "xUnitAssert.h"

namespace xUnitpp
{

Fact::Fact(std::function<void()> fact, const std::string &name, const std::string &suite, const std::string &filename, int line)
    : mFact(fact)
    , mTestDetails(name, suite, filename, line)
{
}

Fact::Fact(const Fact &other)
    : mFact(other.mFact)
    , mTestDetails(other.mTestDetails)
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
    swap(f0.mTestDetails, f1.mTestDetails);
}

const std::string &Fact::Suite() const
{
    return mTestDetails.Suite;
}

void Fact::Run()
{
    try
    {
        mFact();
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
