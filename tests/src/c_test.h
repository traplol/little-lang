#ifndef _C_TEST_C_TEST_H
#define _C_TEST_C_TEST_H

/*
** The MIT License (MIT)
**
** Copyright (c) 2015 Max Mickey
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
** THE SOFTWARE.
*/

#include <stdio.h>
#include <string.h>

#ifndef C_TEST_NO_COLORS

#define ANSI_COLOR_RED      "\x1b[31m"
#define ANSI_COLOR_GREEN    "\x1b[32m"
#define ANSI_COLOR_YELLOW   "\x1b[33m"
#define ANSI_COLOR_BLUE     "\x1b[34m"
#define ANSI_COLOR_MAGENTA  "\x1b[35m"
#define ANSI_COLOR_CYAN     "\x1b[36m"
#define ANSI_COLOR_RESET    "\x1b[0m"

#define PUTS_PASS " : " ANSI_COLOR_GREEN "PASS" ANSI_COLOR_RESET
#define PUTS_FAIL " : " ANSI_COLOR_RED "FAIL" ANSI_COLOR_RESET
#define PRINTF_FAIL ANSI_COLOR_RED "assertion failed " ANSI_COLOR_RESET "in %s at " ANSI_COLOR_CYAN "L:%d" ANSI_COLOR_RESET "\n\t%s\n"

#else

#define PUTS_PASS " : PASS"
#define PUTS_FAIL " : FAIL"
#define PRINTF_FAIL "assertion failed in %s at L:%d\n\t%s\n"

#endif

#define _c_test_XSTR(s) _c_test_STR(s)
#define _c_test_STR(s) #s
#define _c_test_GLUE(a,b) a ## b
#define _c_test_GLUE3(a,b,c) a ## b ## c

#define _c_test_PASS(func)                                              \
    do {                                                                \
        if (_c_test_current_test_passed)                                \
            puts(_c_test_XSTR(_c_test_GLUE(test_, func)) PUTS_PASS);    \
        else                                                            \
            puts(_c_test_XSTR(_c_test_GLUE(test_, func)) PUTS_FAIL);    \
    } while (0)


#define _c_test_TEST_FAIL_MSG(msg)                                      \
    printf(PRINTF_FAIL, __func__, __LINE__, msg)

static int _c_test_current_test_passed = 1;

#define _c_test_fail _c_test_current_test_passed = 0
#define _c_test_pass _c_test_current_test_passed = _c_test_current_test_passed ? 1 : 0


#define _c_test_reset_test_passed _c_test_current_test_passed = 1;

#define _c_test_check_predicate(predicate, msg) \
    do {                                        \
        if (predicate) {                        \
            _c_test_pass;                       \
            break;                              \
        }                                       \
        else {                                  \
            _c_test_fail;                       \
            _c_test_TEST_FAIL_MSG(msg);         \
        }                                       \
    } while (0)

#define _c_test_assert(predicate, msg) do { _c_test_check_predicate(predicate, msg); } while (0)

#define autofail() _c_test_assert(0, "Autofail triggered.")
#define fail(msg) _c_test_assert(0, msg)

#define assert_p(p, msg) _c_test_assert(p, msg)

#define assert_eq(e, a, msg) _c_test_assert(e == a, msg)
#define assert_ne(e, a, msg) _c_test_assert(e != a, msg)
#define assert_lt(l, r, msg) _c_test_assert(l < r, msg)
#define assert_lte(l, r, msg) _c_test_assert(l <= r, msg)
#define assert_gt(l, r, msg) _c_test_assert(l > r, msg)
#define assert_gte(l, r, msg) _c_test_assert(l >= r, msg)

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
