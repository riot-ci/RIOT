/*
 * Copyright (C) 2018 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "crypto/ciphers.h"
#include "crypto/modes/ocb.h"
#include "tests-crypto.h"

/* Test vectors from RFC 7253, Appendix A */
/* The key (K) has a fixed value, the tag length is
   128 bits, and the nonce (N) increments.

     K : 000102030405060708090A0B0C0D0E0F
 */
static uint8_t TEST_KEY[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
};
static uint8_t TEST_KEY_LEN = 16;

static uint8_t TEST_TAG_LEN = 16;

/* Test 1:
    N: BBAA99887766554433221100
    A:
    P:
    C: 785407BFFFC8AD9EDCC5520AC9111EE6
 */

static uint8_t TEST_1_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x00,
};
static size_t TEST_1_NONCE_LEN = 12;

static uint8_t *TEST_1_INPUT;
static size_t TEST_1_INPUT_LEN = 0;

static uint8_t *TEST_1_ADATA;
static size_t TEST_1_ADATA_LEN = 0;

static uint8_t TEST_1_EXPECTED[] = {
    0x78, 0x54, 0x07, 0xBF, 0xFF, 0xC8, 0xAD, 0x9E,
    0xDC, 0xC5, 0x52, 0x0A, 0xC9, 0x11, 0x1E, 0xE6
};
static size_t TEST_1_EXPECTED_LEN = 16;


/* Test 2:
    N: BBAA99887766554433221101
    A: 0001020304050607
    P: 0001020304050607
    C: 6820B3657B6F615A5725BDA0D3B4EB3A257C9AF1F8F03009
 */

static uint8_t TEST_2_NONCE[] = {
    0xBB, 0xAA, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44,
    0x33, 0x22, 0x11, 0x01,
};
static size_t TEST_2_NONCE_LEN = 12;

static uint8_t TEST_2_INPUT[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07    
};
static size_t TEST_2_INPUT_LEN = sizeof(TEST_2_INPUT);

static uint8_t TEST_2_ADATA[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
};
static size_t TEST_2_ADATA_LEN = sizeof(TEST_2_ADATA);

static uint8_t TEST_2_EXPECTED[] = {
    0x68, 0x20, 0xB3, 0x65, 0x7B, 0x6F, 0x61, 0x5A,
    0x57, 0x25, 0xBD, 0xA0, 0xD3, 0xB4, 0xEB, 0x3A,
    0x25, 0x7C, 0x9A, 0xF1, 0xF8, 0xF0, 0x30, 0x09
};
static size_t TEST_2_EXPECTED_LEN = sizeof(TEST_2_EXPECTED);


/* Share test buffer output */
static uint8_t data[60];

static void test_encrypt_op(uint8_t *key, uint8_t key_len,
                            uint8_t *adata, size_t adata_len,
                            uint8_t *nonce, uint8_t nonce_len,
                            uint8_t *plain, size_t plain_len,
                            uint8_t *output_expected,
                            size_t output_expected_len,
                            uint8_t tag_length)
{
    cipher_t cipher;
    int len, err, cmp;

    TEST_ASSERT_MESSAGE(sizeof(data) >= output_expected_len,
                        "Output buffer too small");

    err = cipher_init(&cipher, CIPHER_AES_128, key, key_len);
    TEST_ASSERT_EQUAL_INT(1, err);

    len = cipher_encrypt_ocb(&cipher, adata, adata_len,
                             tag_length, nonce, nonce_len,
                             plain, plain_len, data);
    TEST_ASSERT_MESSAGE(len > 0, "Encryption failed");

    TEST_ASSERT_EQUAL_INT(output_expected_len, len);
    cmp = compare(output_expected, data, len);
    TEST_ASSERT_MESSAGE(1 == cmp, "wrong ciphertext");
}

#define do_test_encrypt_op(name) do { \
        test_encrypt_op(TEST_KEY, TEST_KEY_LEN, \
                        TEST_ ## name ## _ADATA, TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _NONCE, TEST_ ## name ## _NONCE_LEN, \
                    \
                        TEST_ ## name ## _INPUT, TEST_ ## name ## _INPUT_LEN, \
                    \
                        TEST_ ## name ## _EXPECTED + TEST_ ## name ## _ADATA_LEN, \
                        TEST_ ## name ## _EXPECTED_LEN - TEST_ ## name ## _ADATA_LEN, \
                    \
                        TEST_TAG_LEN \
                        ); \
} while (0)

static void test_crypto_modes_ocb_encrypt(void)
{
    do_test_encrypt_op(1);
    do_test_encrypt_op(2);
}

Test *tests_crypto_modes_ocb_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_crypto_modes_ocb_encrypt),
        // new_TestFixture(test_crypto_modes_ccm_decrypt),
        // new_TestFixture(test_crypto_modes_ccm_check_len),
    };

    EMB_UNIT_TESTCALLER(crypto_modes_ocb_tests, NULL, NULL, fixtures);

    return (Test *)&crypto_modes_ocb_tests;
}
