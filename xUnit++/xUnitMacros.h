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
                static std::string name = #name; \
                return name; \
            } \
        } \
    } \
    namespace name ## _xUnitSuite

#define TIMED_FACT(FactName, timeout) \
    void FactName(); \
    namespace FactName ## _ns { xUnitpp::TestCollection::Register reg(&FactName, #FactName, xUnitSuite::Name(), timeout, __FILE__, __LINE__); } \
    void FactName()

#define FACT(FactName) TIMED_FACT(FactName, -1)

#define TIMED_THEORY(TheoryName, timeout, ...) \
    void TheoryName(__VA_ARGS__); \
    std::vector<std::tuple<__VA_ARGS__>> TheoryName ## _data(); \
    namespace TheoryName ## _ns { xUnitpp::TestCollection::Register reg(&TheoryName, &TheoryName ## _data, #TheoryName, xUnitSuite::Name(), timeout, __FILE__, __LINE__); } \
    std::vector<std::tuple<__VA_ARGS__>> TheoryName ## _data()

#define THEORY(TheoryName, ...) TIMED_THEORY(TheoryName, -1, __VA_ARGS__)

#define TIMED_FACT_FIXTURE(FactName, FixtureType, timeout) \
    namespace FactName ## _ns { \
        class FactName ## _Fixture : public FixtureType \
        { \
        public: \
            void FactName(); \
        } FactName ## _instance; \
        xUnitpp::TestCollection::Register reg(std::bind(&FactName ## _Fixture::FactName, FactName ## _instance), #FactName, xUnitSuite::Name(), timeout, __FILE__, __LINE__); \
    } \
    void FactName ## _ns::FactName ## _Fixture::FactName()

#define FACT_FIXTURE(FactName, FixtureType) TIMED_FACT_FIXTURE(FactName, FixtureType, -1)

#endif
