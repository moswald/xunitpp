#include "TestCollection.h"

namespace xUnitpp
{

TestCollection::Register::Register(const std::function<void()> &fn, const std::string &name, const std::string &suite, const std::string &filename, int line)
{
    TestCollection::Instance().mFacts.emplace_back(Fact(fn, name, suite, filename, line));
}

}
