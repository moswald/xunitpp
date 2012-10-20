#ifndef TESTASSEMBLY_H_
#define TESTASSEMBLY_H_

#if defined(WIN32)
#include <Windows.h>

// thanks, Microsoft. "ReportEvent" isn't likely to be used anywhere else, ever
#undef ReportEvent

#endif

#include <string>
#include "xUnit++/ExportApi.h"

namespace xUnitpp { namespace Utilities
{

class TestAssembly
{
private:
    typedef bool (TestAssembly::*bool_type)() const;
    bool is_valid() const;

#if !defined(WIN32)
    typedef void * HMODULE;
#endif

public:
    TestAssembly(const std::string &file, bool shadowCopy);
    ~TestAssembly();

    // !!!VS enable this when Visual Studio supports it
    //explicit operator bool() const
    //{
    //    return module != nullptr;
    //}

    operator bool_type() const;

    xUnitpp::EnumerateTestDetails EnumerateTestDetails;
    xUnitpp::FilteredTestsRunner FilteredTestsRunner;

private:
    HMODULE module;
    std::string tempFile;
    bool shadowCopied;
};

}}

#endif
