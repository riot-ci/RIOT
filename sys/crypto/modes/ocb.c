/*
 * Copyright (C) 2018 Mathias Tausig
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ocb.c
 * @brief       Offset Codebook (OCB3) AEAD mode as specified in RFC 7253
 *
 * @author      Mathias Tausig <mathias@tausig.at>
 *
 */

#include "crypto/modes/ocb.h"
#include <stdint.h>
#include <string.h>

static void double_block(uint8_t source[16], uint8_t dest[16])
{
    uint8_t msb = source[0] >> 7;

    for (uint8_t i = 0; i < 15; ++i) {
        dest[i] = source[i] << 1 | source[i + 1] >> 7;
    }
    dest[15] = (source[15] << 1) ^ (0x87 * msb);
}

static size_t ntz(size_t n)
{
    /* ntz must only be run on positive values */
    if (n == 0) {
        return SIZE_MAX;
    }

    size_t ret = 0;

    while (n % 2 == 0) {
        ++ret;
        n = n >> 1;
    }
    return ret;
}

static void calculate_l_i(uint8_t l_zero[16], size_t i, uint8_t output[16])
{
    memcpy(output, l_zero, 16);
    while ((i--) > 0) {
        double_block(output, output);
    }
}

static void xor_block(uint8_t block1[16], uint8_t block2[16], uint8_t output[16])
{
    for (uint8_t i = 0; i < 16; ++i) {
        output[i] = block1[i] ^ block2[i];
    }
}

static void hash(cipher_t *cipher, uint8_t l_star[16], uint8_t l_zero[16],
                 uint8_t *data, size_t data_len, uint8_t output[16])
{

    /* Calculate the number of full blocks in data */
    size_t m = (data_len - (data_len % 16)) / 16;
    size_t remaining_data_len = data_len - m * 16;


    /* Sum_0 = zeros(128) */
    memset(output, 0, 16);
    /* Offset_0 = zeros(128) */
    uint8_t offset[16];
    memset(offset, 0, 16);
    for (size_t i = 0; i < m; ++i) {
        /* Offset_i = Offset_{i-1} xor L_{ntz(i)} */
        uint8_t l_i[16];
        calculate_l_i(l_zero, ntz(i + 1), l_i);
        xor_block(offset, l_i, offset);
        /* Sum_i = Sum_{i-1} xor ENCIPHER(K, A_i xor Offset_i) */
        uint8_t enciphered_block[16], cipher_input[16];
        xor_block(data, offset, cipher_input);
        cipher->interface->encrypt(&(cipher->context), cipher_input, enciphered_block);
        xor_block(output, enciphered_block, output);

        data += 16;
    }
    if (remaining_data_len > 0) {
        /* Offset_* = Offset_m xor L_* */
        xor_block(offset, l_star, offset);
        /* CipherInput = (A_* || 1 || zeros(127-bitlen(A_*))) xor Offset_* */
        uint8_t cipher_input[16];
        memset(cipher_input, 0, 16);
        memcpy(cipher_input, data, remaining_data_len);
        cipher_input[remaining_data_len] = 0x80;
        xor_block(cipher_input, offset, cipher_input);
        /* Sum = Sum_m xor ENCIPHER(K, CipherInput) */
        uint8_t enciphered_block[16];
        cipher->interface->encrypt(&(cipher->context), cipher_input, enciphered_block);
        xor_block(output, enciphered_block, output);
    }
}

int cipher_encrypt_ocb(cipher_t *cipher, uint8_t *auth_data, uint32_t auth_data_len,
                       uint8_t tag_length, uint8_t *nonce, size_t nonce_len,
                       uint8_t *input, size_t input_len, uint8_t *output)
{

    /* OCB mode only works for ciphers of block length 16 */
    if (cipher->interface->block_size != 16) {
        return OCB_ERR_INVALID_BLOCK_LENGTH;
    }

    /* The tag can be at most 128 bit long */
    if (tag_length > 16) {
        return OCB_ERR_INVALID_TAG_LENGTH;
    }

    /* The nonce can be at most 120 bit long */
    if (nonce_len >= 16) {
        return OCB_ERR_INVALID_NONCE_LENGTH;
    }

    /* Key-dependent variables

       L_* = ENCIPHER(K, zeros(128))
       L_$ = double(L_*)
       L_0 = double(L_$)
       L_i = double(L_{i-1}) for every integer i > 0
     */
    uint8_t l_star[16], l_dollar[16], l_zero[16];
    uint8_t zero_block[16];
    memset(zero_block, 0, 16);
    cipher->interface->encrypt(&(cipher->context), zero_block, l_star);
    double_block(l_star, l_dollar);
    double_block(l_dollar, l_zero);

    /* Calculate the number of full blocks in data */
    size_t m = (input_len - (input_len % 16)) / 16;
    size_t remaining_input_len = input_len - m * 16;

    /* Nonce-dependent and per-encryption variables */
    /* Nonce = num2str(TAGLEN mod 128,7) || zeros(120-bitlen(N)) || 1 || N */
    uint8_t nonce_padded[16];
    memset(nonce_padded, 0, 16);
    nonce_padded[0] = (tag_length * 8) << 1;
    nonce_padded[15 - nonce_len] = 0x01;
    memcpy(nonce_padded + 16 - nonce_len, nonce, nonce_len);

    /* bottom = str2num(Nonce[123..128])*/
    uint8_t bottom = (nonce_padded[15] << 2) >> 2;
    /* Ktop = ENCIPHER(K, Nonce[1..122] || zeros(6)) */
    nonce_padded[15] = nonce_padded[15] & 0xC0;
    uint8_t ktop[16];
    cipher->interface->encrypt(&(cipher->context), nonce_padded, ktop);

    /* Stretch = Ktop || (Ktop[1..64] xor Ktop[9..72]) */
    uint8_t stretch[24];
    memcpy(stretch, ktop, 16);
    for (uint8_t i = 0; i < 8; ++i) {
        stretch[16 + i] = ktop[i] ^ ktop[i + 1];
    }

    /* Offset_0 = Stretch[1+bottom..128+bottom] */
    uint8_t offset[16];
    uint8_t offset_start_byte = bottom / 8;
    uint8_t offset_start_bit = bottom - offset_start_byte * 8;
    for (uint8_t i = 0; i < 16; ++i) {
        offset[i] = (stretch[offset_start_byte + i] << offset_start_bit) | (stretch[offset_start_byte + i + 1] >> (8 - offset_start_bit));
    }

    /* Checksum_0 = zeros(128) */
    uint8_t checksum[16];
    memset(checksum, 0, 16);

    /* Process any whole blocks */
    size_t output_pos = 0;
    for (size_t i = 0; i < m; ++i) {
        /* Offset_i = Offset_{i-1} xor L_{ntz(i)} */
        uint8_t l_i[16];
        calculate_l_i(l_zero, ntz(i + 1), l_i);
        xor_block(offset, l_i, offset);
        /* C_i = Offset_i xor ENCIPHER(K, P_i xor Offset_i) */
        uint8_t ci[16], cipher_input[16];
        xor_block(input, offset, cipher_input);
        cipher->interface->encrypt(&(cipher->context), cipher_input, ci);
        xor_block(ci, offset, ci);
        memcpy(output + output_pos, ci, 16);
        output_pos += 16;
        /* Checksum_i = Checksum_{i-1} xor P_i */
        xor_block(checksum, input, checksum);

        input += 16;
    }

    /* Process any final partial block and compute raw tag */
    uint8_t tag[16];
    if (remaining_input_len > 0) {
        /* Offset_* = Offset_m xor L_* */
        xor_block(offset, l_star, offset);

        /* Pad = ENCIPHER(K, Offset_*) */
        uint8_t pad[16];
        cipher->interface->encrypt(&(cipher->context), offset, pad);

        /* C_* = P_* xor Pad[1..bitlen(P_*)] */
        uint8_t c_star[remaining_input_len];
        memcpy(c_star, pad, remaining_input_len);
        for (uint8_t i = 0; i < remaining_input_len; ++i) {
            c_star[i] = input[i] ^ pad[i];
        }
        memcpy(output + output_pos, c_star, remaining_input_len);
        output_pos += remaining_input_len;

        /* Checksum_* = Checksum_m xor (P_* || 1 || zeros(127-bitlen(P_*))) */
        uint8_t padded_block[16];
        memset(padded_block, 0, 16);
        memcpy(padded_block, input, remaining_input_len);
        padded_block[remaining_input_len] = 0x80;
        xor_block(checksum, padded_block, checksum);

        /* Tag = ENCIPHER(K, Checksum_* xor Offset_* xor L_$) xor HASH(K,A) */
        uint8_t hash_value[16];
        hash(cipher, l_star, l_zero, auth_data, auth_data_len, hash_value);
        uint8_t cipher_data[16];
        xor_block(checksum, offset, cipher_data);
        xor_block(cipher_data, l_dollar, cipher_data);

        cipher->interface->encrypt(&(cipher->context), cipher_data, tag);
        xor_block(tag, hash_value, tag);
    }
    else {
        /* C_* = <empty string>
           Tag = ENCIPHER(K, Checksum_m xor Offset_m xor L_$) xor HASH(K,A)
         */
        uint8_t hash_value[16];
        hash(cipher, l_star, l_zero, auth_data, auth_data_len, hash_value);
        uint8_t cipher_data[16];
        xor_block(checksum, offset, cipher_data);
        xor_block(cipher_data, l_dollar, cipher_data);

        cipher->interface->encrypt(&(cipher->context), cipher_data, tag);
        xor_block(tag, hash_value, tag);
    }
    /* C = C_1 || C_2 || ... || C_m || C_* || Tag[1..TAGLEN] */
    memcpy(output + output_pos, tag, tag_length);
    return input_len + tag_length;
}
