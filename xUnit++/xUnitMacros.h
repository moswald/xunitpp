#ifndef XUNITMACROS_H_
#define XUNITMACROS_H_

#include <tuple>
#include <vector>
#include "TestCollection.h"
#include "Suite.h"

#define SUITE(name) \
    namespace name ## _xUnitSuite { \
        namespace xUnitSuite { \
            inline const std::string &Name() \
            { \
                static std::string name = ""; \
                return name; \
            } \
        } \
    } \
    namespace name ## _xUnitSuite

#define FACT(fn) \
    void fn(); \
    namespace { namespace fn ## _ns { xUnitpp::TestCollection::Register reg(&fn, #fn, xUnitSuite::Name(), __FILE__, __LINE__); } } \
    void fn()

#define THEORY_DATA(theory, ...) \
    void theory(__VA_ARGS__); \
    std::vector<std::tuple<__VA_ARGS__>> theory ## _data(); \
    namespace { namespace theory ## _ns { xUnitpp::TestCollection::Register reg(&theory, &theory ## _data, #theory, xUnitSuite::Name(), __FILE__, __LINE__); } } \
    std::vector<std::tuple<__VA_ARGS__>> theory ## _data()

#endif
