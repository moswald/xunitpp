#ifndef XUNITMACROS_H_
#define XUNITMACROS_H_

#include <tuple>
#include <vector>
#include "Attributes.h"
#include "LineInfo.h"
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
        }; \
        void FactName ## _runner() { FactName ## _Fixture().FactName(); } \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), &FactName ## _runner, #FactName, xUnitSuite::Name(), xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__); \
    } \
    void FactName ## _ns::FactName ## _Fixture::FactName()

#define FACT_FIXTURE(FactName, FixtureType) TIMED_FACT_FIXTURE(FactName, FixtureType, -1)

#define TIMED_FACT(FactName, timeout) TIMED_FACT_FIXTURE(FactName, xUnitpp::NoFixture, timeout)

#define FACT(FactName) TIMED_FACT_FIXTURE(FactName, xUnitpp::NoFixture, -1)

#define TIMED_DATA_THEORY(TheoryName, params, DataProvider, timeout) \
    namespace TheoryName ## _ns { \
        void TheoryName params; \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), TheoryName, DataProvider, #TheoryName, xUnitSuite::Name(), xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__); \
    } \
    void TheoryName ## _ns::TheoryName params

#define DATA_THEORY(TheoryName, params, DataProvider) TIMED_DATA_THEORY(TheoryName, params, DataProvider, -1)

#define TIMED_THEORY(TheoryName, params, timeout, ...) \
    namespace TheoryName ## _ns { \
        void TheoryName params; \
        decltype(FIRST_ARG(__VA_ARGS__)) args[] = { __VA_ARGS__ }; \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), TheoryName, xUnitpp::TheoryData(PP_NARGS(__VA_ARGS__), args), #TheoryName, xUnitSuite::Name(), xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__); \
    } \
    void TheoryName ## _ns::TheoryName params

#define THEORY(TheoryName, params, ...) TIMED_THEORY(TheoryName, params, -1, __VA_ARGS__)

#define THEORY_PROVIDER(DataProvider, ...) \
    std::vector<std::tuple<__VA_ARGS__>> DataProvider() \
    { \
        std::vector<std::tuple<__VA_ARGS__>> data;

#define THEORY_DATA(...) data.emplace_back(std::make_tuple(__VA_ARGS__));

#define THEORY_PROVIDER_END \
    }

#define LI xUnitpp::LineInfo(__FILE__, __LINE__)

#endif
