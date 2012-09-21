#ifndef XUNITMACROS_H_
#define XUNITMACROS_H_

#include <memory>
#include <tuple>
#include <vector>
#include "Attributes.h"
#include "LineInfo.h"
#include "TestCollection.h"
#include "TestDetails.h"
#include "Suite.h"
#include "xUnitCheck.h"
#include "xUnitMacroHelpers.h"

#define ATTRIBUTES(...) \
    namespace XU_UNIQUE_ATT_NS { \
        namespace xUnitAttributes { \
            inline xUnitpp::AttributeCollection Attributes() \
            { \
                xUnitpp::AttributeCollection attributes; \
                XU_ATTRIBUTES(__VA_ARGS__) \
                return attributes; \
            } \
        } \
    } \
    namespace XU_UNIQUE_ATT_NS

#define SUITE(SuiteName) \
    namespace { \
        namespace xUnitSuite { \
            inline const std::string &Name() \
            { \
                static std::string name = SuiteName; \
                return name; \
            } \
        } \
    } \
    namespace

namespace xUnitpp { struct NoFixture {}; }

#define TIMED_FACT_FIXTURE(FactDetails, FixtureType, timeout) \
    namespace XU_UNIQUE_NS { \
        using xUnitpp::Assert; \
        std::shared_ptr<xUnitpp::Check> pCheck = std::make_shared<xUnitpp::Check>(); \
        class XU_UNIQUE_FIXTURE : public FixtureType \
        { \
            /* !!!VS fix when '= delete' is supported */ \
            XU_UNIQUE_FIXTURE &operator =(XU_UNIQUE_FIXTURE) /* = delete */; \
        public: \
            XU_UNIQUE_FIXTURE() : Check(*pCheck) { } \
            void XU_UNIQUE_TEST(); \
            const xUnitpp::Check &Check; \
        }; \
        void XU_UNIQUE_RUNNER() { XU_UNIQUE_FIXTURE().XU_UNIQUE_TEST(); } \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), \
            &XU_UNIQUE_RUNNER, FactDetails, xUnitSuite::Name(), \
            xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__, pCheck); \
    } \
    void XU_UNIQUE_NS :: XU_UNIQUE_FIXTURE :: XU_UNIQUE_TEST()

#define UNTIMED_FACT_FIXTURE(FactDetails, FixtureType) TIMED_FACT_FIXTURE(FactDetails, FixtureType, 0)

#define FACT_FIXTURE(FactDetails, FixtureType) TIMED_FACT_FIXTURE(FactDetails, FixtureType, -1)

#define TIMED_FACT(FactDetails, timeout) TIMED_FACT_FIXTURE(FactDetails, xUnitpp::NoFixture, timeout)

#define UNTIMED_FACT(FactDetails) TIMED_FACT_FIXTURE(FactDetails, xUnitpp::NoFixture, 0)

#define FACT(FactDetails) TIMED_FACT_FIXTURE(FactDetails, xUnitpp::NoFixture, -1)

#define TIMED_DATA_THEORY(TheoryName, params, DataProvider, timeout) \
    namespace TheoryName ## _ns { \
        using xUnitpp::Assert; \
        std::shared_ptr<xUnitpp::Check> pCheck = std::make_shared<xUnitpp::Check>(); \
        xUnitpp::Check &Check = *pCheck; \
        void TheoryName params; \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), \
            TheoryName, DataProvider, #TheoryName, xUnitSuite::Name(), \
            xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__, pCheck); \
    } \
    void TheoryName ## _ns::TheoryName params

#define UNTIMED_DATA_THEORY(TheoryName, params, DataProvider, timeout) TIMED_DATA_THEORY(TheoryName, params, DataProvider, 0)

#define DATA_THEORY(TheoryName, params, DataProvider) TIMED_DATA_THEORY(TheoryName, params, DataProvider, -1)

#define TIMED_THEORY(TheoryName, params, timeout, ...) \
    namespace TheoryName ## _ns { \
        using xUnitpp::Assert; \
        std::shared_ptr<xUnitpp::Check> pCheck = std::make_shared<xUnitpp::Check>(); \
        xUnitpp::Check &Check = *pCheck; \
        void TheoryName params; \
        decltype(FIRST_ARG(__VA_ARGS__)) args[] = { __VA_ARGS__ }; \
        xUnitpp::TestCollection::Register reg(xUnitpp::TestCollection::Instance(), \
            TheoryName, xUnitpp::TheoryData(PP_NARGS(__VA_ARGS__), args), #TheoryName, \
            xUnitSuite::Name(), xUnitAttributes::Attributes(), timeout, __FILE__, __LINE__, pCheck); \
    } \
    void TheoryName ## _ns::TheoryName params

#define UNTIMED_THEORY(TheoryName, params, ...) TIMED_THEORY(TheoryName, params, 0, __VA_ARGS__)

#define THEORY(TheoryName, params, ...) TIMED_THEORY(TheoryName, params, -1, __VA_ARGS__)

#define LI xUnitpp::LineInfo(__FILE__, __LINE__)

#endif
