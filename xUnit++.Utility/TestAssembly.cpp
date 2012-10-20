#include "TestAssembly.h"

#if !defined(WIN32)
#include <cstdio>
#include <dlfcn.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace
{
#if defined(WIN32)
    std::string CopyFile(const std::string &file)
    {
        char tempPath[MAX_PATH] = {0};
        if (GetTempPath(MAX_PATH, tempPath) > 0)
        {
            if (GetTempFileName(tempPath, "xU+", 0, tempPath) != 0)
            {
                if (::CopyFile(file.c_str(), tempPath, FALSE))
                {
                    return tempPath;
                }
            }
        }

        return "";
    }
#else
    std::string CopyFile(const std::string &file)
    {
        std::string result;

        char buf[BUFSIZ];
        size_t size;

        char tempFile[] = "/tmp/xU+XXXXXX";

        int source = open(file.c_str(), O_RDONLY, 0);
        int dest = mkstemp(tempFile);

        if (source > 0)
        {
            if (dest > 0)
            {
                while ((size = read(source, buf, BUFSIZ)) > 0)
                {
                    write(dest, buf, size);
                }

                result = tempFile;

                close(dest);
            }

            close(source);
        }

        return result;
    }
#endif
}

namespace xUnitpp { namespace Utilities
{

TestAssembly::TestAssembly(const std::string &file, bool shadowCopy)
    : EnumerateTestDetails(nullptr)
    , FilteredTestsRunner(nullptr)
    , module(nullptr)
    , tempFile(shadowCopy ? CopyFile(file) : file)
    , shadowCopied(shadowCopy)
{
    if (!tempFile.empty())
    {
#if defined(WIN32)
        if ((module = LoadLibrary(tempFile.c_str())) != nullptr)
        {
            EnumerateTestDetails = (xUnitpp::EnumerateTestDetails)GetProcAddress(module, "EnumerateTestDetails");
            FilteredTestsRunner = (xUnitpp::FilteredTestsRunner)GetProcAddress(module, "FilteredTestsRunner");
        }
#else
        if ((module = dlopen(tempFile.c_str(), RTLD_LAZY)) != nullptr)
        {
            // so silly. ISO C++ forbids casting void * to a pointer-to-function
            // but POSIX says "sure, go right ahead"
            // this weird syntax works around that
            *(void **)(&EnumerateTestDetails) = dlsym(module, "EnumerateTestDetails");
            *(void **)(&FilteredTestsRunner) = dlsym(module, "FilteredTestsRunner");
        }
#endif
    }
}

TestAssembly::~TestAssembly()
{
    if (module)
    {
#if defined(WIN32)
        FreeLibrary(module);
#else
        dlclose(module);
#endif
    }

    if (!tempFile.empty() && shadowCopied)
    {
#if defined(WIN32)
        DeleteFile(tempFile.c_str());
#else
        std::remove(tempFile.c_str());
#endif
    }
}

bool TestAssembly::is_valid() const
{
    return EnumerateTestDetails != nullptr && FilteredTestsRunner != nullptr;
}

TestAssembly::operator bool_type() const
{
    return is_valid() ? &TestAssembly::is_valid : nullptr;
}

}}
