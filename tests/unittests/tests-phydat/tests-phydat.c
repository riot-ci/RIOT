/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include "tests-phydat.h"

#include "phydat.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void test_phydat_fit(void)
{
    int val0 = 100000;
    int val1 = 2000000;
    int val2 = 30000000;
    int val4 = 1234567;
    phydat_t dat;
    dat.scale = -6;
    dat.unit = UNIT_V;
    int res = phydat_fit(&dat, val0, 0, 0);
    TEST_ASSERT_EQUAL_INT(1, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-5, dat.scale);
    TEST_ASSERT_EQUAL_INT( 10000, dat.val[0]);
    res = phydat_fit(&dat, val1, 1, res);
    TEST_ASSERT_EQUAL_INT(2, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-4, dat.scale);
    TEST_ASSERT_EQUAL_INT(  1000, dat.val[0]);
    TEST_ASSERT_EQUAL_INT( 20000, dat.val[1]);
    res = phydat_fit(&dat, val2, 2, res);
    TEST_ASSERT_EQUAL_INT(3, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-3, dat.scale);
    TEST_ASSERT_EQUAL_INT(   100, dat.val[0]);
    TEST_ASSERT_EQUAL_INT(  2000, dat.val[1]);
    TEST_ASSERT_EQUAL_INT( 30000, dat.val[2]);
    res = phydat_fit(&dat, val4, 1, res);
    TEST_ASSERT_EQUAL_INT(3, res);
    TEST_ASSERT_EQUAL_INT(UNIT_V, dat.unit);
    TEST_ASSERT_EQUAL_INT(-3, dat.scale);
    TEST_ASSERT_EQUAL_INT(   100, dat.val[0]);
    TEST_ASSERT_EQUAL_INT(  1235, dat.val[1]);
    TEST_ASSERT_EQUAL_INT( 30000, dat.val[2]);
}

Test *tests_phydat_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_phydat_fit),
    };

    EMB_UNIT_TESTCALLER(phydat_tests, NULL, NULL, fixtures);

    return (Test *)&phydat_tests;
}

void tests_phydat(void)
{
    TESTS_RUN(tests_phydat_tests());
}
