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

#define TIMED_THEORY(TheoryName, params, DataProvider, timeout) \
    void TheoryName params; \
    namespace TheoryName ## _ns { xUnitpp::TestCollection::Register reg(TheoryName, DataProvider, #TheoryName, xUnitSuite::Name(), timeout, __FILE__, __LINE__); } \
    void TheoryName params

#define THEORY(TheoryName, params, DataProvider) TIMED_THEORY(TheoryName, params, DataProvider, -1)

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
