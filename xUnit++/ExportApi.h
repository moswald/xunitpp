#ifndef EXPORTSAPI_H_
#define EXPORTSAPI_H_

#include <functional>
#include <memory>
#include <vector>

namespace xUnitpp
{
    struct IOutput;
    struct TestDetails;

    typedef int(*FilteredTestsRunner)(int, std::shared_ptr<IOutput>, std::function<bool(const TestDetails &)>);
    typedef void(*ListAllTests)(std::vector<TestDetails> &tests);
}

#endif
