/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 *
 * @author      Martine Lenders <m.lenders@fu-berlin.de>
 *
 * @}
 */

#include <errno.h>
#include <stdio.h>
#include "net/sock/udp.h"

#define TEST_PORT               (38664U)
#define TEST_REMOTE             { 0xfe, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 }
#define TEST_PAYLOAD            { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef }

static const uint8_t _test_remote[] = TEST_REMOTE;
static const uint8_t _test_payload[] = TEST_PAYLOAD;

static sock_udp_t _udp_sock;

int main(void)
{
    int res;
    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_ep_t remote = SOCK_IPV6_EP_ANY;

    local.port = TEST_PORT;
    sock_udp_create(&_udp_sock, &local, NULL, 0);


    memcpy(remote.addr.ipv6, _test_remote, sizeof(_test_remote));
    remote.port = TEST_PORT - 1;

    /* remote is not reachable, so it should return an error */
    res = sock_udp_send(&_udp_sock, _test_payload, sizeof(_test_payload), &remote);
    if (res == -EHOSTUNREACH) {
        printf("SUCCESS: error code EHOSTUNREACH (%li == %i)\n", (long)(-res),
               EHOSTUNREACH);
        return 0;
    }
    else {
        printf("FAILURE: gnrc_udp_send() had an unexpected error code: %li\n",
               (long int)res);
        return 1;
    }
}

/** @} */
