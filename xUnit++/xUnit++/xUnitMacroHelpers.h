#ifndef XUNITMACROHELPERS_H_
#define XUNITMACROHELPERS_H_

#define XU_CAT2(a,b) a ## b
#define XU_CAT(a,b) XU_CAT2(a,b)

#define XU_UNIQUE_ATT_NS XU_CAT(TestAtt_, __LINE__)
#define XU_UNIQUE_NS XU_CAT(TestNs_, __LINE__)
#define XU_UNIQUE_FIXTURE XU_CAT(TestFixture_, __LINE__)
#define XU_UNIQUE_TEST XU_CAT(TestFn_, __LINE__)
#define XU_UNIQUE_RUNNER XU_CAT(TestRunner_, __LINE__)

// !!!VS fix when initializer lists are supported
#define XU_TEST_EVENTS \
namespace detail \
{ \
    std::shared_ptr<xUnitpp::TestEventRecorder> eventRecorders[] = { \
        /* check */ std::make_shared<xUnitpp::TestEventRecorder>(), \
        /* warn  */ std::make_shared<xUnitpp::TestEventRecorder>(), \
        /* log   */ std::make_shared<xUnitpp::TestEventRecorder>(), \
    }; \
    auto pCheck = std::make_shared<xUnitpp::Check>(*eventRecorders[0]); \
    auto pWarn = std::make_shared<xUnitpp::Warn>(*eventRecorders[1]); \
    auto pLog = std::make_shared<xUnitpp::Log>(*eventRecorders[2]); \
} \
std::vector<std::shared_ptr<xUnitpp::TestEventRecorder>> eventRecorders(std::begin(detail::eventRecorders), std::end(detail::eventRecorders)); \

// with thanks for various sources, but I got it from
// http://stackoverflow.com/questions/2308243/macro-returning-the-number-of-arguments-it-is-given-in-c

//#define PP_RSEQ_N() \
//    63,62,61,60,                   \
//    59,58,57,56,55,54,53,52,51,50, \
//    49,48,47,46,45,44,43,42,41,40, \
//    39,38,37,36,35,34,33,32,31,30, \
//    29,28,27,26,25,24,23,22,21,20, \
//    19,18,17,16,15,14,13,12,11,10, \
//     9, 8, 7, 6, 5, 4, 3, 2, 1, 0
//
//#define PP_ARG_N( \
//     _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
//    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
//    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
//    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
//    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
//    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
//    _61,_62,_63,  N, ...) N
//
//#define PP_NARG_(...) \
//    PP_ARG_N(__VA_ARGS__)
//
//#define PP_NARG(...) \
//    PP_NARG_(__VA_ARGS__,PP_RSEQ_N())

// ...and a little VS help from
// http://stackoverflow.com/questions/5530505/variadic-macro-argument-count-macro-fails-in-vs2010-but-works-on-ideone

#define EXPAND(x) x

#define PP_NARGS(...) \
    EXPAND(_xPP_NARGS_IMPL(__VA_ARGS__,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0))

#define _xPP_NARGS_IMPL(x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,N,...) N

#define CAR(x, ...) x
#define CDR(x, ...) __VA_ARGS__

// attribute expansion helpers
#define XU_A_1(a) \
    attributes.insert(std::make_pair a);
#define XU_A_2(a, b) \
    XU_A_1(a) \
    XU_A_1(b)
#define XU_A_3(a, b, c) \
    XU_A_1(a) \
    XU_A_2(b, c)
#define XU_A_4(a, b, c, d) \
    XU_A_1(a) \
    XU_A_3(b, c, d)
#define XU_A_5(a, b, c, d, e) \
    XU_A_1(a) \
    XU_A_4(b, c, d, e)
#define XU_A_6(a, b, c, d, e, f) \
    XU_A_1(a) \
    XU_A_5(b, c, d, e, f)
#define XU_A_7(a, b, c, d, e, f, g) \
    XU_A_1(a) \
    XU_A_6(b, c, d, e, f, g)
#define XU_A_8(a, b, c, d, e, f, g, h) \
    XU_A_1(a) \
    XU_A_7(b, c, d, e, f, g, h)
#define XU_ATT(M, ...) M(__VA_ARGS__)
#define XU_ATTRIBUTES(...) XU_ATT(XU_CAT(XU_A_, PP_NARGS(__VA_ARGS__)), __VA_ARGS__)

#endif
