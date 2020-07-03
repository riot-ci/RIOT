/*
 * Copyright (C) 2014 Hochschule für Angewandte Wissenschaften Hamburg (HAW)
 * Copyright (C) 2014 Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys_base64
 * @{
 * @file
 * @brief   Functions to encode and decode base64
 *
 * @author  Martin Landsmann <Martin.Landsmann@HAW-Hamburg.de>
 * @}
 *
 */

#include <stdbool.h>
#include <stdint.h>

#include "base64.h"
#include "kernel_defines.h"

#define BASE64_CAPITAL_UPPER_BOUND     (25)     /**< base64 'Z'           */
#define BASE64_SMALL_UPPER_BOUND       (51)     /**< base64 'z'           */
#define BASE64_NUMBER_UPPER_BOUND      (61)     /**< base64 '9'           */
#define BASE64_PLUS                    (62)     /**< base64 '+'           */
#define BASE64_MINUS                   (62)     /**< base64 '-'           */
#define BASE64_SLASH                   (63)     /**< base64 '/'           */
#define BASE64_UNDERLINE               (63)     /**< base64 '_'           */
#define BASE64_EQUALS                  (0xFE)   /**< no base64 symbol '=' */
#define BASE64_NOT_DEFINED             (0xFF)   /**< no base64 symbol     */

/*
 * returns the corresponding ascii symbol value for the given base64 code
 */
static char getsymbol(uint8_t code, bool urlsafe)
{
    if (!IS_ACTIVE(MODULE_BASE64URL)) {
        urlsafe = false;
    }

    if (urlsafe && code == BASE64_UNDERLINE) {
        return '_';
    }

    if (urlsafe && code == BASE64_MINUS) {
        return '-';
    }

    if (!urlsafe && code == BASE64_SLASH) {
        return '/';
    }

    if (!urlsafe && code == BASE64_PLUS) {
        return '+';
    }

    if (code <= BASE64_CAPITAL_UPPER_BOUND) {
        return (code + 'A');
    }

    if (code <= BASE64_SMALL_UPPER_BOUND) {
        return (code + ('z' - BASE64_SMALL_UPPER_BOUND));
    }

    if (code <= BASE64_NUMBER_UPPER_BOUND) {
        return (code + ('9' - BASE64_NUMBER_UPPER_BOUND));
    }

    return (char)BASE64_NOT_DEFINED;
}

static void encode_three_bytes(uint8_t *dest,
                               uint8_t b1, uint8_t b2, uint8_t b3,
                               bool urlsafe)
{
    dest[0] = getsymbol(b1 >> 2, urlsafe);
    dest[1] = getsymbol(((b1 & 0x03) << 4) | (b2 >> 4), urlsafe);
    dest[2] = getsymbol(((b2 & 0x0f) << 2) | (b3 >> 6), urlsafe);
    dest[3] = getsymbol(b3 & 0x3f, urlsafe);
}

static int base64_encode_base(const void *data_in, size_t data_in_size,
                              void *base64_out, size_t *base64_out_size,
                              bool urlsafe)
{
    const uint8_t *in = data_in;
    const uint8_t *end = in + data_in_size;
    uint8_t *out = base64_out;
    size_t required_size = base64_estimate_encode_size(data_in_size);

    if (in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (data_in_size == 0) {
        *base64_out_size = 0;
        return BASE64_SUCCESS;
    }

    if (*base64_out_size < required_size) {
        *base64_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    *base64_out_size = required_size;

    while (in < end - 2) {
        encode_three_bytes(out, in[0], in[1], in[2], urlsafe);
        out += 4;
        in += 3;
    }

    if (in == end) {
        /* data_in_size is multiple of 3, we're done */
        return BASE64_SUCCESS;
    }

    if (in + 1 == end) {
        /* One byte still left to decode, set other two input bytes to zero */
        encode_three_bytes(out, in[0], 0, 0, urlsafe);
        /* Replace last two bytes with "=" to signal corresponding input bytes
         * didn't exist */
        out[2] = out[3] = '=';
        return BASE64_SUCCESS;
    }

    /* Final case: 2 bytes remain for encoding, use zero as third input */
    encode_three_bytes(out, in[0], in[1], 0, urlsafe);
    /* Replace last output with "=" to signal corresponding input byte didn't exit */
    out[3] = '=';

    return BASE64_SUCCESS;
}

int base64_encode(const void *data_in, size_t data_in_size,
                  void *base64_out, size_t *base64_out_size)
{
    return base64_encode_base(data_in, data_in_size, base64_out, base64_out_size, false);
}

#if IS_ACTIVE(MODULE_BASE64URL)
int base64url_encode(const void *data_in, size_t data_in_size,
                     void *base64_out, size_t *base64_out_size)
{
    return base64_encode_base(data_in, data_in_size, base64_out, base64_out_size, true);
}
#endif

/*
 *  returns the corresponding base64 code for the given ascii symbol
 */
static uint8_t getcode(char symbol)
{
    if (symbol == '/') {
        return BASE64_SLASH;
    }

    if (symbol == '_') {
        return BASE64_UNDERLINE;
    }

    if (symbol == '+') {
        return BASE64_PLUS;
    }

    if (symbol == '-') {
        return BASE64_MINUS;
    }

    if (symbol == '=') {
        /* indicates a padded base64 end */
        return BASE64_EQUALS;
    }

    if (symbol < '0') {
        /* indicates that the given symbol is not base64 and should be ignored */
        return BASE64_NOT_DEFINED;
    }

    if (symbol <= '9' && symbol >= '0') {
        return (symbol + (BASE64_NUMBER_UPPER_BOUND - '9'));
    }

    if (symbol <= 'Z' && symbol >= 'A') {
        return (symbol - 'A');
    }

    if (symbol <= 'z' && symbol >= 'a') {
        return (symbol + (BASE64_SMALL_UPPER_BOUND - 'z'));
    }

    /* indicates that the given symbol is not base64 and should be ignored */
    return BASE64_NOT_DEFINED;
}

static void decode_four_codes(uint8_t *out, const uint8_t *src)
{
    out[0] = (src[0] << 2) | (src[1] >> 4);
    out[1] = (src[1] << 4) | (src[2] >> 2);
    out[2] = (src[2] << 6) | src[3];
}

int base64_decode(const void *base64_in, size_t base64_in_size,
                  void *data_out, size_t *data_out_size)
{
    uint8_t *out = data_out;
    const uint8_t *in = base64_in;
    size_t required_size = base64_estimate_decode_size(base64_in_size);

    if (in == NULL) {
        return BASE64_ERROR_DATA_IN;
    }

    if (*data_out_size < required_size) {
        *data_out_size = required_size;
        return BASE64_ERROR_BUFFER_OUT_SIZE;
    }

    if (data_out == NULL) {
        return BASE64_ERROR_BUFFER_OUT;
    }

    const uint8_t *end = base64_in + base64_in_size;
    uint8_t codes[4];
    size_t n_codes;

    while (1) {
        n_codes = 0;
        do {
            if (in == end) {
                switch (n_codes) {
                    case 0:
                        /* nothing to do */
                        break;
                    case 1:
                        return BASE64_ERROR_DATA_IN_SIZE;
                    case 2:
                        codes[2] = codes[3] = 0;
                        decode_four_codes(out, codes);
                        out += 1;
                        break;
                    case 3:
                        codes[3] = 0;
                        decode_four_codes(out, codes);
                        out += 2;
                        break;
                }
                *data_out_size = (size_t)out - (size_t)data_out;
                return BASE64_SUCCESS;
            }
            switch (codes[n_codes] = getcode(*in++)) {
                case BASE64_NOT_DEFINED:
                case BASE64_EQUALS:
                    continue;
            }
            n_codes++;
        }
        while (n_codes < 4);

        decode_four_codes(out, codes);
        out += 3;
    }
}
