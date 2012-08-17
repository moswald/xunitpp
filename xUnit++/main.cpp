#include "xUnitTestRunner.h"

int main()
{
    return xUnitpp::RunAllTests(std::chrono::milliseconds(50));
}
