/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"

#include "cib.h"

#include "tests-core.h"

#define TEST_CIB_SIZE  2

static cib_t cib;

static void set_up(void)
{
    cib_init(&cib, TEST_CIB_SIZE);
}

static void test_cib_put(void)
{
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_put(&cib));
}

static void test_cib_get(void)
{
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
}

static void test_cib_peek(void)
{
    cib_init(&cib, TEST_CIB_SIZE);
    TEST_ASSERT_EQUAL_INT(-1, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_peek(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_peek(&cib));
}

static void test_cib_avail(void)
{
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(2, cib_avail(&cib));
}

static void test_cib_put_and_get(void)
{
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_put(&cib));
}

static void test_empty_cib(void)
{
    cib_init(&cib, 0);
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_put(&cib));
}

static void test_overflow_cib(void)
{
    cib_init(&cib, 4);
    cib.read_count = 0xffffffff;
    cib.write_count = 0xffffffff;
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(3, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(3, cib_get(&cib));
}

static void test_singleton_cib(void)
{
    cib_init(&cib, 1);
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(-1, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_put(&cib));
    TEST_ASSERT_EQUAL_INT(1, cib_avail(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_get(&cib));
    TEST_ASSERT_EQUAL_INT(0, cib_avail(&cib));
}

Test *tests_core_cib_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_cib_put),
        new_TestFixture(test_cib_get),
        new_TestFixture(test_cib_avail),
        new_TestFixture(test_cib_put_and_get),
        new_TestFixture(test_empty_cib),
        new_TestFixture(test_overflow_cib),
        new_TestFixture(test_singleton_cib),
        new_TestFixture(test_cib_peek),
    };

    EMB_UNIT_TESTCALLER(core_cib_tests, set_up, NULL, fixtures);

    return (Test *)&core_cib_tests;
}
