#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

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

    typedef void (*ListAllTests)(std::vector<std::string> &tests);
    ListAllTests listAllTests = (ListAllTests)GetProcAddress(testlib, "ListAllTests");

    if (listAllTests == nullptr)
    {
        std::cerr << "unable to get ListAllTests" << std::endl;
        return -1;
    }

    std::vector<std::string> tests;
    listAllTests(tests);

    for (auto t : tests)
    {
        std::cout << t << std::endl;
    }
}
