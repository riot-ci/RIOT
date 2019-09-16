/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <limits.h>
#include <stdint.h>

#include "embUnit.h"

#include "bitarithm.h"

#include "tests-core.h"

static void test_SETBIT_null_null(void)
{
    int res = 0x00;

    SETBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_SETBIT_null_limit(void)
{
    unsigned int res = 0x00;

    SETBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_SETBIT_limit_null(void)
{
    unsigned int res = UINT_MAX;

    SETBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_SETBIT_limit_limit(void)
{
    unsigned int res = UINT_MAX;

    SETBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_SETBIT_null_one(void)
{
    unsigned int res = 0x00;

    SETBIT(res, 0x01);

    TEST_ASSERT_EQUAL_INT(0x01, res);
}

static void test_SETBIT_one_null(void)
{
    unsigned int res = 0x01;

    SETBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x01, res);
}

static void test_SETBIT_one_random(void)
{
    unsigned int res = 0x01;

    SETBIT(res, 0x06);  /* randomized by fair dice roll ;-) */

    TEST_ASSERT_EQUAL_INT(0x07, res);
}

static void test_CLRBIT_null_null(void)
{
    int res = 0x00;

    CLRBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_null_limit(void)
{
    unsigned int res = 0x00;

    CLRBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_limit_null(void)
{
    unsigned int res = UINT_MAX;

    CLRBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(UINT_MAX, res);
}

static void test_CLRBIT_limit_limit(void)
{
    unsigned int res = UINT_MAX;

    CLRBIT(res, UINT_MAX);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_null_one(void)
{
    unsigned int res = 0x00;

    CLRBIT(res, 0x01);

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_CLRBIT_one_null(void)
{
    unsigned int res = 0x01;

    CLRBIT(res, 0x00);

    TEST_ASSERT_EQUAL_INT(0x01, res);
}

static void test_CLRBIT_one_random(void)
{
    unsigned int res = 0x01;

    CLRBIT(res, 0x05);  /* randomized by fair dice roll ;-) */

    TEST_ASSERT_EQUAL_INT(0x00, res);
}

static void test_bitarithm_msb_one(void)
{
    TEST_ASSERT_EQUAL_INT(0, bitarithm_msb(1));
}

static void test_bitarithm_msb_limit(void)
{
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned) * 8 - 1,
                          bitarithm_msb(UINT_MAX));
}

static void test_bitarithm_msb_random(void)
{
    TEST_ASSERT_EQUAL_INT(4, bitarithm_msb(19)); /* randomized by fair
                                                  * dice roll ;-)
                                                  */
}

static void test_bitarithm_msb_16bit(void)
{
    for (unsigned i = 1; i < UINT16_MAX; i++) {
        TEST_ASSERT_EQUAL_INT(((sizeof(unsigned) * 8) - __builtin_clz(i) - 1), bitarithm_msb(i));
    }
}

static void test_bitarithm_lsb_one(void)
{
    TEST_ASSERT_EQUAL_INT(0, bitarithm_lsb(1));
}

static void test_bitarithm_lsb_limit(void)
{
    unsigned shift = sizeof(unsigned) * 8 - 1;
    TEST_ASSERT_EQUAL_INT(shift, bitarithm_lsb(1u << shift));
}

static void test_bitarithm_lsb_random(void)
{
    TEST_ASSERT_EQUAL_INT(3, bitarithm_lsb(24)); /* randomized by fair
                                                          dice roll ;-) */
}

static void test_bitarithm_lsb_all(void)
{
    for (unsigned i = 1; i < UINT16_MAX; i++) {
        TEST_ASSERT_EQUAL_INT(__builtin_ctz(i), bitarithm_lsb(i));
    }
}

static void test_bitarithm_bits_set_null(void)
{
    TEST_ASSERT_EQUAL_INT(0, bitarithm_bits_set(0));
}

static void test_bitarithm_bits_set_one(void)
{
    TEST_ASSERT_EQUAL_INT(1, bitarithm_bits_set(1));
}

static void test_bitarithm_bits_set_limit(void)
{
    TEST_ASSERT_EQUAL_INT(sizeof(unsigned) * 8,
                          bitarithm_bits_set(UINT_MAX));
}

static void test_bitarithm_bits_set_random(void)
{
    TEST_ASSERT_EQUAL_INT(3, bitarithm_bits_set(7)); /* randomized by fair
                                                      * dice roll ;-)
                                                      */
}

static void test_bitarithm_bits_set_u32_random(void)
{
    TEST_ASSERT_EQUAL_INT(21, bitarithm_bits_set_u32(4072524027)); /* Source: https://www.random.org/bytes */
}

static void test_bitarithm_set_masked_8(void)
{
    uint8_t val = 0;

    bitarithm_set_masked(&val, 0x18, 3);
    TEST_ASSERT_EQUAL_INT(0x18, val);

    bitarithm_set_masked(&val, 0x18, 3);
    TEST_ASSERT_EQUAL_INT(0x18, val);

    bitarithm_set_masked(&val, 0x18, 2);
    TEST_ASSERT_EQUAL_INT(0x10, val);

    bitarithm_set_masked(&val, 0x18, 1);
    TEST_ASSERT_EQUAL_INT(0x8, val);

    bitarithm_set_masked(&val, 0x18, 0);
    TEST_ASSERT_EQUAL_INT(0x0, val);

    val = 0x81;
    bitarithm_set_masked(&val, 0x18, 11);
    TEST_ASSERT_EQUAL_INT(0x99, val);

    bitarithm_set_masked(&val, 0xff, 0);
    TEST_ASSERT_EQUAL_INT(0x0, val);
}

static void test_bitarithm_set_masked_16(void)
{
    uint16_t val = 0x2300;

    bitarithm_set_masked(&val, 0x18, 3);
    TEST_ASSERT_EQUAL_INT(0x2318, val);

    bitarithm_set_masked(&val, 0x18, 3);
    TEST_ASSERT_EQUAL_INT(0x2318, val);

    bitarithm_set_masked(&val, 0x18, 2);
    TEST_ASSERT_EQUAL_INT(0x2310, val);

    bitarithm_set_masked(&val, 0x18, 1);
    TEST_ASSERT_EQUAL_INT(0x2308, val);

    bitarithm_set_masked(&val, 0x18, 0);
    TEST_ASSERT_EQUAL_INT(0x2300, val);
}

static void test_bitarithm_set_masked_32(void)
{
    uint32_t val = 0xfefe2300;

    bitarithm_set_masked(&val, 0x18, 3);
    TEST_ASSERT_EQUAL_INT(0xfefe2318, val);

    bitarithm_set_masked(&val, 0x18, 3);
    TEST_ASSERT_EQUAL_INT(0xfefe2318, val);

    bitarithm_set_masked(&val, 0x18, 2);
    TEST_ASSERT_EQUAL_INT(0xfefe2310, val);

    bitarithm_set_masked(&val, 0x18, 1);
    TEST_ASSERT_EQUAL_INT(0xfefe2308, val);

    bitarithm_set_masked(&val, 0x18, 0);
    TEST_ASSERT_EQUAL_INT(0xfefe2300, val);
}

static void test_bitarithm_get_masked(void)
{
    TEST_ASSERT_EQUAL_INT(0x4, bitarithm_get_masked(0xA5, 0x38));
    TEST_ASSERT_EQUAL_INT(0x3, bitarithm_get_masked(0xA9AA, 0x180));
    TEST_ASSERT_EQUAL_INT(0x42, bitarithm_get_masked(0x55542AAA, 0xFF000));
}

Test *tests_core_bitarithm_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_SETBIT_null_null),
        new_TestFixture(test_SETBIT_null_limit),
        new_TestFixture(test_SETBIT_limit_null),
        new_TestFixture(test_SETBIT_limit_limit),
        new_TestFixture(test_SETBIT_null_one),
        new_TestFixture(test_SETBIT_one_null),
        new_TestFixture(test_SETBIT_one_random),

        new_TestFixture(test_CLRBIT_null_null),
        new_TestFixture(test_CLRBIT_null_limit),
        new_TestFixture(test_CLRBIT_limit_null),
        new_TestFixture(test_CLRBIT_limit_limit),
        new_TestFixture(test_CLRBIT_null_one),
        new_TestFixture(test_CLRBIT_one_null),
        new_TestFixture(test_CLRBIT_one_random),

        new_TestFixture(test_bitarithm_msb_one),
        new_TestFixture(test_bitarithm_msb_limit),
        new_TestFixture(test_bitarithm_msb_random),
        new_TestFixture(test_bitarithm_msb_16bit),

        new_TestFixture(test_bitarithm_lsb_one),
        new_TestFixture(test_bitarithm_lsb_limit),
        new_TestFixture(test_bitarithm_lsb_random),
        new_TestFixture(test_bitarithm_lsb_all),

        new_TestFixture(test_bitarithm_bits_set_null),
        new_TestFixture(test_bitarithm_bits_set_one),
        new_TestFixture(test_bitarithm_bits_set_limit),
        new_TestFixture(test_bitarithm_bits_set_random),
        new_TestFixture(test_bitarithm_bits_set_u32_random),

        new_TestFixture(test_bitarithm_set_masked_8),
        new_TestFixture(test_bitarithm_set_masked_16),
        new_TestFixture(test_bitarithm_set_masked_32),
        new_TestFixture(test_bitarithm_get_masked),
    };

    EMB_UNIT_TESTCALLER(core_bitarithm_tests, NULL, NULL, fixtures);

    return (Test *)&core_bitarithm_tests;
}
