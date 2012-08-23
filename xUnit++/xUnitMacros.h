#ifndef XUNITMACROS_H_
#define XUNITMACROS_H_

#include <tuple>
#include <vector>
#include "Attributes.h"
#include "TestCollection.h"
#include "TestDetails.h"
#include "Suite.h"
#include "xUnitMacroHelpers.h"

#define ATTRIBUTES(TestName, ...) \
    namespace TestName ## _ns { \
        namespace xUnitAttributes { \
            inline xUnitpp::AttributeCollection Attributes() \
            { \
                xUnitpp::AttributeCollection attributes; \
                XU_ATTRIBUTES(__VA_ARGS__) \
                return attributes; \
            } \
        } \
    }

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

namespace xUnitpp { struct NoFixture {}; }

#define TIMED_FACT_FIXTURE(FactName, FixtureType, timeout) \
    namespace FactName ## _ns { \
        class FactName ## _Fixture : public FixtureType \
        { \
        public: \
            void FactName(); \
        } FactName ## _instance; \
        xUnitpp::TestCollection::Register reg(std::bind(&FactName ## _Fixture::FactName, FactName ## _instance), #FactName, xUnitSuite::Name(), xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__); \
    } \
    void FactName ## _ns::FactName ## _Fixture::FactName()

#define FACT_FIXTURE(FactName, FixtureType) TIMED_FACT_FIXTURE(FactName, FixtureType, -1)

#define TIMED_FACT(FactName, timeout) TIMED_FACT_FIXTURE(FactName, xUnitpp::NoFixture, timeout)

#define FACT(FactName) TIMED_FACT_FIXTURE(FactName, xUnitpp::NoFixture, -1)

#define TIMED_THEORY(TheoryName, params, DataProvider, timeout) \
    void TheoryName params; \
    namespace TheoryName ## _ns { xUnitpp::TestCollection::Register reg(TheoryName, DataProvider, #TheoryName, xUnitSuite::Name(), xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__); } \
    void TheoryName params

#define THEORY(TheoryName, params, DataProvider) TIMED_THEORY(TheoryName, params, DataProvider, -1)

#endif
