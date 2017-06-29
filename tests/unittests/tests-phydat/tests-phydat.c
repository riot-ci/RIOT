/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Unittests for the ``phydat`` module
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "embUnit/embUnit.h"

#include "phydat.h"

#include "tests-phydat.h"

#if (PHYDAT_DIM != 3)
#error "PHYDAT unittests are only applicable if PHYDAT_DIM is 3"
#endif

#define BUFSIZE         (128U)

static char test[BUFSIZE];

typedef struct {
    int dim;
    phydat_t dat;
    const char *json;
} tdat_t;

/* define test data */

static tdat_t data[] = {
    {
        .dim  = 1,
        .dat  = { { 2345, 0, 0 }, UNIT_TEMP_C, -2 },
        .json = "{\"d\":23.45,\"u\":\"°C\"}"
    },
    {
        .dim  = 3,
        .dat  = { { 1032, 10, -509 }, UNIT_G, -3 },
        .json = "{\"d\":[1.032,0.010,-0.509],\"u\":\"g\"}"
    },
    {
        .dim  = 3,
        .dat  = { { 1200, 38, 98 }, UNIT_M, -3 },
        .json = "{\"d\":[1.200,0.038,0.098],\"u\":\"m\"}"
    },
    {
        .dim  = 2,
        .dat  = { { 19, 23, 0 }, UNIT_NONE, 0 },
        .json = "{\"d\":[19,23],\"u\":\"none\"}"
    },
    {
        .dim  = 1,
        .dat  = { { 1, 0, 0 }, UNIT_BOOL, 0 },
        .json = "{\"d\":true,\"u\":\"bool\"}"
    },
    {
        .dim  = 3,
        .dat  = { { 1, 0, 1 }, UNIT_BOOL, 0 },
        .json = "{\"d\":[true,false,true],\"u\":\"bool\"}"
    }
};

static void test_unitstr__success(void)
{
    /* just testing some samples from the list of units... */
    TEST_ASSERT_EQUAL_STRING("undefined", phydat_unit_to_str_verbose(UNIT_UNDEF));
    TEST_ASSERT_EQUAL_STRING("none", phydat_unit_to_str_verbose(UNIT_NONE));
    TEST_ASSERT_EQUAL_STRING("bool", phydat_unit_to_str_verbose(UNIT_BOOL));
    TEST_ASSERT_EQUAL_STRING("°C", phydat_unit_to_str_verbose(UNIT_TEMP_C));
    TEST_ASSERT_EQUAL_STRING("Bar", phydat_unit_to_str_verbose(UNIT_BAR));
    TEST_ASSERT_EQUAL_STRING("ppm", phydat_unit_to_str_verbose(UNIT_PPM));
}

static void test_json__success(void)
{
    size_t len;

    len = phydat_to_json(&data[0].dat, data[0].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[0].json) + 1), len);
    len = phydat_to_json(&data[0].dat, data[0].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[0].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[0].json, (const char *)test);

    len = phydat_to_json(&data[1].dat, data[1].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[1].json) + 1), len);
    len = phydat_to_json(&data[1].dat, data[1].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[1].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[1].json, (const char *)test);

    len = phydat_to_json(&data[2].dat, data[2].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[2].json) + 1), len);
    len = phydat_to_json(&data[2].dat, data[2].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[2].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[2].json, (const char *)test);

    len = phydat_to_json(&data[3].dat, data[3].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[3].json) + 1), len);
    len = phydat_to_json(&data[3].dat, data[3].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[3].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[3].json, (const char *)test);

    len = phydat_to_json(&data[4].dat, data[4].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[4].json) + 1), len);
    len = phydat_to_json(&data[4].dat, data[4].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[4].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[4].json, (const char *)test);

    len = phydat_to_json(&data[5].dat, data[5].dim, NULL);
    TEST_ASSERT_EQUAL_INT((strlen(data[5].json) + 1), len);
    len = phydat_to_json(&data[5].dat, data[5].dim, test);
    TEST_ASSERT_EQUAL_INT(strlen(data[5].json), strlen(test));
    TEST_ASSERT_EQUAL_STRING(data[5].json, (const char *)test);
}

Test *tests_phydat_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_unitstr__success),
        new_TestFixture(test_json__success),
    };

    EMB_UNIT_TESTCALLER(phydat_tests, NULL, NULL, fixtures);

    return (Test *)&phydat_tests;
}

void tests_phydat(void)
{
    TESTS_RUN(tests_phydat_tests());
}
/** @} */
