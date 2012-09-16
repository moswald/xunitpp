#ifndef EXPORTSAPI_H_
#define EXPORTSAPI_H_

#include <functional>
#include <memory>
#include <vector>

#if !defined(WIN32)
# define __declspec(x)
#endif

namespace xUnitpp
{
    struct IOutput;
    struct TestDetails;

    typedef std::function<void(const TestDetails &)> EnumerateTestDetailsCallback;
    typedef void(*EnumerateTestDetails)(EnumerateTestDetailsCallback callback);

    typedef std::function<bool(const TestDetails &)> TestFilterCallback;
    typedef int(*FilteredTestsRunner)(int, int, IOutput &, TestFilterCallback);
}

#endif
