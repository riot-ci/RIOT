/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include "tests-analog_util.h"

#include "analog_util.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef struct {
    int expected;
    int sample;
    int min;
    int max;
    adc_res_t res;
} test_values_t;

/* Arbitrarily chosen test vectors */
/* TODO: Choose test vectors in a more qualified manner to catch any edge cases */
static test_values_t test_data[] = {
    {     0,     0,          0,      10000, ADC_RES_16BIT},
    {  1000,     0,       1000,          0, ADC_RES_16BIT},
    { 65535, 65535,          0,      65536, ADC_RES_16BIT},
    { 32768,   128,          0,      65536,  ADC_RES_8BIT},
    {  8192,   128,          0,      65536, ADC_RES_10BIT},
    {   256,     1,          0,      65536,  ADC_RES_8BIT},
    { 65280,   255,          0,      65536,  ADC_RES_8BIT},
    {  1039,    10,       1000,       2000,  ADC_RES_8BIT},
    { 17324,  3000,      10000,      20000, ADC_RES_12BIT},
    { 11831,  3000,      10000,      20000, ADC_RES_14BIT},
    {  2301,  3000,         13,      50000, ADC_RES_16BIT},
    {  -134, 56789,      -1000,          0, ADC_RES_16BIT},
    { 16062, 45671,      30000,      10000, ADC_RES_16BIT},
    { -2535, 30000,     -30000,      30000, ADC_RES_16BIT},
    {     0, 65535,      65535,          0, ADC_RES_16BIT},
    { 65534,     1,      65535,          0, ADC_RES_16BIT},
    {  3972,  9876,      10000,          0, ADC_RES_14BIT},
};

#define TEST_DATA_NUMOF (sizeof(test_data) / sizeof(test_data[0]))

static void test_adc_util_map(void)
{
    for (unsigned int k = 0; k < TEST_DATA_NUMOF; ++k) {
        test_values_t *testp = &test_data[k];
        int res = adc_util_map(testp->sample, testp->res, testp->min, testp->max);
        TEST_ASSERT_EQUAL_INT(testp->expected, res);
    }
}

Test *tests_adc_util_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_adc_util_map),
    };

    EMB_UNIT_TESTCALLER(adc_util_tests, NULL, NULL, fixtures);

    return (Test *)&adc_util_tests;
}

void tests_analog_util(void)
{
    TESTS_RUN(tests_adc_util_tests());
}
