#include <iostream>
#include <string>
#include <tuple>
#include <vector>
#include <Windows.h>
#include "TestDetails.h"
#include "xUnitTestRunner.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "usage: xunit.console.exe <test assembly>" << std::endl;
        return -1;
    }

    auto testlib = LoadLibrary(argv[1]);

    if (testlib == nullptr)
    {
        std::cerr << "unable to load " << argv[1] << std::endl;
        return -1;
    }

    typedef void (*ListAllTests)(std::vector<std::tuple<std::string, xUnitpp::AttributeCollection>> &tests);
    ListAllTests listAllTests = (ListAllTests)GetProcAddress(testlib, "ListAllTests");

    if (listAllTests == nullptr)
    {
        std::cerr << "unable to get ListAllTests" << std::endl;
        return -1;
    }

    std::vector<std::tuple<std::string, xUnitpp::AttributeCollection>> tests;
    listAllTests(tests);

    for (const auto &t : tests)
    {
        std::cout << std::get<0>(t) << std::endl;
    
        const auto &attributes = std::get<1>(t);
        for (const auto &a : attributes)
        {
            std::cout << "  [" << a.first << " : " << a.second << "]" << std::endl;
        }
    }
}
