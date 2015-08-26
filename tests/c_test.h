#ifndef _C_TEST_C_TEST_H
#define _C_TEST_C_TEST_H

#include <stdio.h>
#include <string.h>

#define _c_test_XSTR(s) _c_test_STR(s)
#define _c_test_STR(s) #s
#define _c_test_GLUE(a,b) a ## b
#define _c_test_GLUE3(a,b,c) a ## b ## c

#define _c_test_PASS(func)                                              \
    do {                                                                \
        if (_c_test_current_test_passed)                                \
            puts(_c_test_XSTR(_c_test_GLUE(test_, func)) " : PASS"); \
        else                                                            \
            puts(_c_test_XSTR(_c_test_GLUE(test_, func)) " : FAIL");    \
    } while (0)


#define _c_test_TEST_FAIL_MSG(msg)                                      \
    printf("assertion failed in %s at L:%d\n\t%s\n", __func__, __LINE__, msg)

static int _c_test_current_test_passed = 1;

static void _c_test_fail(void) {
    _c_test_current_test_passed = 0;
}

static void _c_test_pass(void) {
    if (!_c_test_current_test_passed)
        return;
    _c_test_current_test_passed = 1;
}

#define _c_test_reset_test_passed _c_test_current_test_passed = 1;

#define _c_test_check_predicate(predicate, msg) \
    do {                                        \
        if (predicate) {                        \
            _c_test_pass();                     \
            break;                              \
        }                                       \
        else {                                  \
            _c_test_fail();                     \
            _c_test_TEST_FAIL_MSG(msg);         \
        }                                       \
    } while (0)

#define _c_test_assert(predicate, msg) do { _c_test_check_predicate(predicate, msg); } while (0)

#define assert_eq(e, a, msg) _c_test_assert(e == a, msg)
#define assert_ne(e, a, msg) _c_test_assert(e != a, msg)
#define assert_lt(e, a, msg) _c_test_assert(e < a, msg)
#define assert_lte(e, a, msg) _c_test_assert(e <= a, msg)
#define assert_gt(e, a, msg) _c_test_assert(e > a, msg)
#define assert_gte(e, a, msg) _c_test_assert(e >= a, msg)

#define assert_str_eq(e, a, msg) _c_test_assert(0 == strcmp(e, a), msg)
#define assert_str_ne(e, a, msg) _c_test_assert(0 != strcmp(e, a), msg)

#define TEST(func) void _c_test_GLUE(test_, func) (void)

#define TEST_RUN(func)                          \
    do {                                        \
        _c_test_reset_test_passed;              \
        _c_test_GLUE(test_, func)();            \
        _c_test_PASS(func);                     \
    } while (0)

#endif
