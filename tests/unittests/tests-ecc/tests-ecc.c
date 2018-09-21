/*
 * Copyright (C) 2015 Lucas Jenß
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @brief
 * @{
 *
 * @brief       Tests for Error Correction Codes
 *
 * @author      Lucas Jenß <lucas@x3ro.de>
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#include <string.h>
#include "embUnit.h"

#include "ecc/hamming256.h"
#include "ecc/golay2412.h"

static void test_hamming256_single(void)
{
    uint8_t data[256];
    uint8_t ecc[3];
    uint8_t result;

    memset(data, 0xAB, 256);

    hamming_compute256x(data, 256, ecc);
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_NONE, result);

    data[10] |= (2 << 3);
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_SINGLEBIT, result);

    data[10] |= (2 << 3);
    data[20] |= (2 << 5);
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_MULTIPLEBITS, result);

    memset(data, 0xAB, 256);
    ecc[1] ^= 1; /* Flip first bit, corrupting the ECC */
    result = hamming_verify256x(data, 256, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_ECC, result);
}

static void test_hamming256_padding(void)
{
    uint8_t data[203];
    uint8_t ecc[3];
    uint8_t result;

    memset(data, 0xAB, 203);

    hamming_compute256x(data, 203, ecc);
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_NONE, result);

    data[10] |= (2 << 3);
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_SINGLEBIT, result);

    data[10] |= (2 << 3);
    data[20] |= (2 << 5);
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_MULTIPLEBITS, result);

    memset(data, 0xAB, 203);
    ecc[1] ^= 1; /* Flip first bit, corrupting the ECC */
    result = hamming_verify256x(data, 203, ecc);
    TEST_ASSERT_EQUAL_INT(Hamming_ERROR_ECC, result);
}

static void test_golay2412_message(void)
{
    /* source for random bytes: https://www.random.org/bytes */
    unsigned char data[] = {201, 240, 154, 5, 227, 60, 116, 192, 214};
    unsigned char result[sizeof(data)];
    unsigned char msg_enc[2*sizeof(data)];

    golay2412_encode(sizeof(data), &data[0], &msg_enc[0]);

    /* Add errors here. golay(24, 12) can correct up to 3 errors in one 24-bit
     * symbol (= 3 bytes). Positions for bitflips generated at
     * https://www.random.org/bytes */
    msg_enc[0]  ^= (1 << 14) | (1 << 13) | (1 <<  5);
    msg_enc[3]  ^= (1 <<  0) | (1 << 21) | (1 << 18);
    msg_enc[6]  ^= (1 << 16) | (1 << 22) | (1 << 19);
    msg_enc[9]  ^= (1 << 11) | (1 <<  8) | (1 << 15);
    msg_enc[12] ^= (1 <<  9) | (1 <<  2) | (1 << 10);
    msg_enc[15] ^= (1 << 17) | (1 <<  1) | (1 <<  7);

    golay2412_decode(sizeof(data), &msg_enc[0], &result[0]);

    TEST_ASSERT_EQUAL_INT(0, memcmp(&data, &result, sizeof(data)));
}

TestRef test_all(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_hamming256_single),
        new_TestFixture(test_hamming256_padding),
        new_TestFixture(test_golay2412_message),
    };

    EMB_UNIT_TESTCALLER(EccTest, NULL, NULL, fixtures);
    return (TestRef) & EccTest;
}

void tests_ecc(void)
{
    TESTS_RUN(test_all());
}
