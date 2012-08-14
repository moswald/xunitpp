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

namespace xUnitpp { struct NoFixture {}; }

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

#define TIMED_FACT(FactName, timeout) TIMED_FACT_FIXTURE(FactName, xUnitpp::NoFixture, timeout)

#define FACT(FactName) TIMED_FACT_FIXTURE(FactName, xUnitpp::NoFixture, -1)

#define TIMED_THEORY(TheoryName, params, DataProvider, timeout) \
    void TheoryName params; \
    namespace TheoryName ## _ns { xUnitpp::TestCollection::Register reg(TheoryName, DataProvider, #TheoryName, xUnitSuite::Name(), timeout, __FILE__, __LINE__); } \
    void TheoryName params

#define THEORY(TheoryName, params, DataProvider) TIMED_THEORY(TheoryName, params, DataProvider, -1)

#define ASSERT_EQUAL(expected, actual, comparer_or_precision)
//#define ASSERT_EQUAL(expected, actual)
#define ASSERT_NOT_EQUAL(expected, actual, comparer)
//#define ASSERT_NOT_EQUAL(expected, actual)
#define ASSERT_NO_THROW(expression)
#define ASSERT_THROWS(expression)
#define ASSERT_FAIL(message)
#define ASSERT_FALSE(check, ...)
#define ASSERT_TRUE(check, ...)
#define ASSERT_EMPTY(container)
#define ASSERT_NOT_EMPTY(container)
#define ASSERT_DOES_NOT_CONTAIN(container, item)
#define ASSERT_CONTAINS(container, item)
#define ASSERT_IN_RANGE(value, min, max)
#define ASSERT_NOT_IN_RANGE(value, min, max)
#define ASSERT_NOT_NULL(value)
#define ASSERT_NULL(value)
#define ASSERT_NOT_SAME(value0, value1)
#define ASSERT_SAME(value0, value1)

#endif
