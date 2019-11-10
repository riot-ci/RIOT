/*
 * AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 *
 * Copyright (c) 2003-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "crypto/includes.h"

#include "crypto/common.h"
#include "crypto/aes.h"
#include "crypto/aes_wrap.h"

/**
 * aes_wrap - Wrap keys with AES Key Wrap Algorithm (128-bit KEK) (RFC3394)
 * @kek: 16-octet Key encryption key (KEK)
 * @n: Length of the plaintext key in 64-bit units; e.g., 2 = 128-bit = 16
 * bytes
 * @plain: Plaintext key to be wrapped, n * 64 bits
 * @cipher: Wrapped key, (n + 1) * 64 bits
 * Returns: 0 on success, -1 on failure
 */
int  wpa_aes_wrap(const u8 *kek, int n, const u8 *plain, u8 *cipher)
{
    u8 *a, *r, b[16];
    int i, j;
    void *ctx;

    a = cipher;
    r = cipher + 8;

    /* 1) Initialize variables. */
    os_memset(a, 0xa6, 8);
    os_memcpy(r, plain, 8 * n);

    ctx = wpa_aes_encrypt_init(kek, 16);
    if (ctx == NULL)
        return -1;

    /* 2) Calculate intermediate values.
     * For j = 0 to 5
     *     For i=1 to n
     *         B = AES(K, A | R[i])
     *         A = MSB(64, B) ^ t where t = (n*j)+i
     *         R[i] = LSB(64, B)
     */
    for (j = 0; j <= 5; j++) {
        r = cipher + 8;
        for (i = 1; i <= n; i++) {
            os_memcpy(b, a, 8);
            os_memcpy(b + 8, r, 8);
            wpa_aes_encrypt(ctx, b, b);
            os_memcpy(a, b, 8);
            a[7] ^= n * j + i;
            os_memcpy(r, b + 8, 8);
            r += 8;
        }
    }
    wpa_aes_encrypt_deinit(ctx);

    /* 3) Output the results.
     *
     * These are already in @cipher due to the location of temporary
     * variables.
     */

    return 0;
}
