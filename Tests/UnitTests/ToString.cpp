#include "xUnit++/xUnit++.h"

SUITE("ToString")
{

DATA_THEORY("ToString should not move parameters", (const std::string &s),
    []() -> std::vector<std::tuple<std::string>>
    {
        std::vector<std::tuple<std::string>> data;
        data.emplace_back("ABCD");
        return data;
    })
{
    Assert.Equal("ABCD", s);
}

}
