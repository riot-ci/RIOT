/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       minimal RIOT application, intended as size test
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

#include "random.h"
#include "sign.h"

static const unsigned char m[] = "0123456789abcdef";
static unsigned char sm[64 + sizeof(m)];
static unsigned char m_result[sizeof(m)];

int main(void)
{
    unsigned char sk[64];
    unsigned char pk[32];

    puts("creating keys...");
    random_bytes(sk, 32);
    keypair(pk, sk);

    puts("signing...");

    unsigned long long smlen;
    sign(sm, &smlen, m, sizeof(m), pk, sk);

    puts("verifying...");

    if (verify(m_result, 0, sm, smlen, pk) == 0) {
        printf("m=\"%s\"\n", m_result);
        puts("ok!");
    }
    else {
        puts("failed.");
    }

    puts("verifying tampered msg...");

    sm[70] = 'x';

    if (verify(m_result, 0, sm, smlen, pk) == 0) {
        printf("m=\"%s\"\n", m_result);
        puts("unexpected ok?");
    }
    else {
        puts("expected failure!");
    }

    puts("done.");

    return 0;
}
