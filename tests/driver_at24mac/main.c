/*
 * Copyright (C) 2019 Benjamin Valentin
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
 * @brief       Test application for the AT24MAC driver
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <stdio.h>
#include "at24mac.h"

int main(void)
{
    eui48_t e48;
    eui64_t e64;
    uint8_t id[AT24MAC_ID_LEN];

    if (at24mac_get_eui48(0, &e48) != 0) {
        puts("[FAILED]");
        return 1;
    }

    if (at24mac_get_eui64(0, &e64) != 0) {
        puts("[FAILED]");
        return 1;
    }

    if (at24mac_get_id128(0, &id) != 0) {
        puts("[FAILED]");
        return 1;
    }

    printf("EUI-48:");
    for (unsigned i = 0; i < sizeof(e48.uint8); ++i) {
        printf(" %02x", e48.uint8[i]);
    }
    puts("");

    printf("EUI-64:");
    for (unsigned i = 0; i < sizeof(e64.uint8); ++i) {
        printf(" %02x", e64.uint8[i]);
    }
    puts("");

    printf("ID-128:");
    for (unsigned i = 0; i < sizeof(id); ++i) {
        printf(" %02x", id[i]);
    }
    puts("");

    puts("[SUCCESS]");

    return 0;
}
