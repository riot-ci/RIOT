/*
 * Copyright (C) 2019 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <string.h>
#include <luid.h>

#include "tests-luid.h"

/* provide custom luid_base to test weak linking */
static const uint8_t hw_mac[] = {
    0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77
};

/* custom implementation of luid_base() */
size_t luid_get_eui64_custom(eui64_t *addr, uint8_t idx)
{
    if (idx > 0) {
        return 0;
    }

    memcpy(addr, hw_mac, sizeof(*addr));
    return sizeof(hw_mac);
}

static void test_luid_is_hw_mac(void)
{
    eui64_t addr;
    luid_get_eui64(&addr);

    TEST_ASSERT_EQUAL_INT(0, addr.uint8[0] & 0x1);
    TEST_ASSERT_EQUAL_INT(2, addr.uint8[0] & 0x2);
    TEST_ASSERT_EQUAL_INT(0, memcmp(addr.uint8 + 1, hw_mac + 1, sizeof(addr) - 1));
}

static void test_luid_uniqe_eui64(void)
{
    eui64_t mac[3];

    luid_get_eui64(&mac[0]);
    luid_get_eui64(&mac[1]);
    luid_get_eui64(&mac[2]);
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[0], &mac[1], sizeof(mac[0])));
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[1], &mac[2], sizeof(mac[1])));
}

static void test_luid_uniqe_eui48(void)
{
    eui48_t mac[3];

    luid_get_eui48(&mac[0]);
    luid_get_eui48(&mac[1]);
    luid_get_eui48(&mac[2]);
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[0], &mac[1], sizeof(mac[0])));
    TEST_ASSERT_EQUAL_INT(0, !memcmp(&mac[1], &mac[2], sizeof(mac[1])));
}

static void test_luid_custom(void)
{
    uint8_t a[2][8];
    uint8_t b[2][8];

    luid_custom(a[0], sizeof(a[0]), 0xfefe);
    luid_custom(a[1], sizeof(a[1]), 0xfefe);
    luid_custom(b[0], sizeof(b[0]), 0xbeef);
    luid_custom(b[1], sizeof(b[1]), 0xbeef);

    TEST_ASSERT_EQUAL_INT(0, !memcmp(a[0], b[0], sizeof(a[0])));
    TEST_ASSERT_EQUAL_INT(0, !memcmp(a[1], b[1], sizeof(a[1])));
    TEST_ASSERT_EQUAL_INT(0, memcmp(a[0], a[0], sizeof(a[0])));
    TEST_ASSERT_EQUAL_INT(0, memcmp(b[1], b[1], sizeof(b[0])));
}

Test *tests_luid_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_luid_is_hw_mac),
        new_TestFixture(test_luid_uniqe_eui48),
        new_TestFixture(test_luid_uniqe_eui64),
        new_TestFixture(test_luid_custom),
    };

    EMB_UNIT_TESTCALLER(luid_tests, NULL, NULL, fixtures);

    return (Test *)&luid_tests;
}

void tests_luid(void)
{
    TESTS_RUN(tests_luid_tests());
}
