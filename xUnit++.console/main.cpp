#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include "CommandLine.h"
#include "TestDetails.h"
#include "xUnitTestRunner.h"

int main(int argc, char **argv)
{
    std::vector<std::string> libraries;

    try
    {
        xUnitpp::Utilities::CommandLine commandLine(argc, argv);
        libraries = commandLine.TestLibraries();
    }
    catch (std::invalid_argument &e)
    {
        std::cerr << e.what();
        return -1;
    }

    int failures = 0;

    for (const auto &lib : libraries)
    {
        auto testlib = LoadLibrary(lib.c_str());

        if (testlib == nullptr)
        {
            std::cerr << "unable to load " << argv[1] << std::endl;
            return -1;
        }

    //typedef void (*ListAllTests)(std::vector<std::tuple<std::string, xUnitpp::AttributeCollection>> &tests);
    //ListAllTests listAllTests = (ListAllTests)GetProcAddress(testlib, "ListAllTests");
    //
    //if (listAllTests == nullptr)
    //{
    //    std::cerr << "unable to get ListAllTests" << std::endl;
    //    return -1;
    //}
    //
    //std::vector<std::tuple<std::string, xUnitpp::AttributeCollection>> tests;
    //listAllTests(tests);
    //
    //for (const auto &t : tests)
    //{
    //    std::cout << std::get<0>(t) << std::endl;
    //
    //    const auto &attributes = std::get<1>(t);
    //    for (const auto &a : attributes)
    //    {
    //        std::cout << "  [" << a.first << " : " << a.second << "]" << std::endl;
    //    }
    //}

        typedef size_t (*RunAll)();
        RunAll runAll = (RunAll)GetProcAddress(testlib, "RunAll");

        if (runAll == nullptr)
        {
            std::cerr << "unable to get RunAll" << std::endl;
            return -1;
        }

        failures += runAll();
    }

    return failures;
}
