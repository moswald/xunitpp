#include "TestCollection.h"
#include "Fact.h"

namespace xUnitpp
{

TestCollection &TestCollection::Instance()
{
    static TestCollection collection;
    return collection;
}

TestCollection::Register::Register(const std::function<void()> &fn, const std::string &name, const std::string &suite, const std::string &filename, int line)
{
    TestCollection::Instance().mFacts.emplace_back(Fact(fn, name, suite, filename, line));
}

const std::vector<Fact> &TestCollection::Facts()
{
    return Instance().mFacts;
}

const std::vector<Theory> &TestCollection::Theories()
{
    return Instance().mTheories;
}

}
