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

#define FACT(FactName) \
    void FactName(); \
    namespace { namespace FactName ## _ns { xUnitpp::TestCollection::Register reg(&FactName, #FactName, xUnitSuite::Name(), __FILE__, __LINE__); } } \
    void FactName()

#define THEORY(TheoryName, ...) \
    void TheoryName(__VA_ARGS__); \
    std::vector<std::tuple<__VA_ARGS__>> TheoryName ## _data(); \
    namespace { namespace TheoryName ## _ns { xUnitpp::TestCollection::Register reg(&TheoryName, &TheoryName ## _data, #TheoryName, xUnitSuite::Name(), __FILE__, __LINE__); } } \
    std::vector<std::tuple<__VA_ARGS__>> TheoryName ## _data()

#define FACT_FIXTURE(FactName, FixtureType) \
    namespace FactName ## _ns { \
        class FactName ## _Fixture : public FixtureType \
        { \
        public: \
            void FactName(); \
        } FactName ## _instance; \
        xUnitpp::TestCollection::Register reg(std::bind(&FactName ## _Fixture::FactName, FactName ## _instance), #FactName, xUnitSuite::Name(), __FILE__, __LINE__); \
    } \
    void FactName ## _ns::FactName ## _Fixture::FactName()

#endif
