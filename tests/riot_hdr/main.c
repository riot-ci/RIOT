/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for module riot_hdr
 *
 * @author     Francisco Acosta <francisco.acosta@inria.fr>
 */

#include <stdio.h>

#include "riot_hdr.h"
#include "embUnit.h"

const riot_hdr_t riot_hdr_good = {
    .magic_number = RIOT_HDR_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum = 0x02eda672
};

const riot_hdr_t riot_hdr_bad_magic = {
    .magic_number = 0x12345678,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum = 0x02eda672
};

const riot_hdr_t riot_hdr_bad_chksum = {
    .magic_number = RIOT_HDR_MAGIC,
    .version = 0x5bd19bff,
    .start_addr = 0x00001100,
    .chksum = 0x02000000
};

static void test_riot_hdr_01(void)
{
    int ret = riot_hdr_validate(&riot_hdr_good);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_riot_hdr_02(void)
{
    int ret = riot_hdr_validate(&riot_hdr_bad_magic);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

static void test_riot_hdr_03(void)
{
    int ret = riot_hdr_validate(&riot_hdr_bad_chksum);
    TEST_ASSERT_EQUAL_INT(-1, ret);
}

static void test_riot_hdr_04(void)
{
    uint32_t chksum = riot_hdr_checksum(&riot_hdr_good);
    TEST_ASSERT_EQUAL_INT(0x02eda672, chksum);
}

static void test_riot_hdr_05(void)
{
    uint32_t chksum = riot_hdr_checksum(&riot_hdr_good);
    TEST_ASSERT_EQUAL_INT(0x02eda672, chksum);
}

Test *tests_riot_hdr(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_riot_hdr_01),
        new_TestFixture(test_riot_hdr_02),
        new_TestFixture(test_riot_hdr_03),
        new_TestFixture(test_riot_hdr_04),
        new_TestFixture(test_riot_hdr_05),
    };

    EMB_UNIT_TESTCALLER(riot_hdr_tests, NULL, NULL, fixtures);

    return (Test *)&riot_hdr_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_riot_hdr());
    TESTS_END();
    return 0;
}
