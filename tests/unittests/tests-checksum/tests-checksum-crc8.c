/*
 * Copyright 2019 Benjamin Valentin <benpicco@googlemail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>

#include "embUnit/embUnit.h"

#include "checksum/crc8.h"

#include "tests-checksum.h"

static void test_checksum_crc8_sequence_empty(void)
{
    unsigned char buf[] = "";
    uint8_t expect = 0xFF;

    TEST_ASSERT_EQUAL_INT(expect ,crc8(buf, sizeof(buf) - 1));
}

static void test_checksum_crc8_sequence_1a(void)
{
    unsigned char buf[] = "A";
    uint8_t expect = 0xA0;

    TEST_ASSERT_EQUAL_INT(expect, crc8(buf, sizeof(buf) - 1));
}

static void test_checksum_crc8_sequence_256a(void)
{
    unsigned char buf[] = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                          "AAAA";
    uint8_t expect = 0xF0;

    TEST_ASSERT_EQUAL_INT(expect, crc8(buf, sizeof(buf) - 1));
}

static void test_checksum_crc8_sequence_1to9(void)
{
    unsigned char buf[] = "123456789";
    uint8_t expect = 0xF7;

    TEST_ASSERT_EQUAL_INT(expect, crc8(buf, sizeof(buf) - 1));
}

static void test_checksum_crc8_sequence_4bytes(void)
{
    unsigned char buf[] = { 0x12, 0x34, 0x56, 0x78 };
    uint8_t expect = 0x8A;

    TEST_ASSERT_EQUAL_INT(expect, crc8(buf, sizeof(buf) - 1));
}

Test *tests_checksum_crc8_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        /* Reference values according to
         * http://srecord.sourceforge.net/crc16-ccitt.html */
        new_TestFixture(test_checksum_crc8_sequence_empty),
        new_TestFixture(test_checksum_crc8_sequence_1a),
        new_TestFixture(test_checksum_crc8_sequence_256a),
        new_TestFixture(test_checksum_crc8_sequence_1to9),
        new_TestFixture(test_checksum_crc8_sequence_4bytes),
    };

    EMB_UNIT_TESTCALLER(checksum_crc8_tests, NULL, NULL, fixtures);

    return (Test *)&checksum_crc8_tests;
}
