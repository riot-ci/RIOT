/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_riot_hdr
 * @{
 *
 * @file
 * @brief       RIOT header helpers and tools
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <string.h>
#include <stddef.h>

#ifdef RIOT_VERSION
#include "log.h"
#else
#include <stdio.h>
#define LOG_INFO(...) printf(__VA_ARGS__)
#endif

#include "riot_hdr.h"
#include "checksum/fletcher32.h"
#include "byteorder.h"

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#   error "This code is implementented in a way that it will only work for little-endian systems!"
#endif

void riot_hdr_print(const riot_hdr_t *riot_hdr)
{
    printf("Image magic_number: 0x%08x\n", (unsigned)riot_hdr->magic_number);
    printf("Image Version: 0x%08x\n", (unsigned)riot_hdr->version);
    printf("Image start address: 0x%08x\n", (unsigned)riot_hdr->start_addr);
    printf("Header chksum: 0x%08x\n", (unsigned)riot_hdr->chksum);
    printf("\n");
}

int riot_hdr_validate(const riot_hdr_t *riot_hdr)
{
    if (riot_hdr->magic_number != RIOT_HDR_MAGIC) {
        LOG_INFO("%s: riot_hdr magic number invalid\n", __func__);
        return -1;
    }

    int res = riot_hdr_checksum(riot_hdr) == riot_hdr->chksum ? 0 : -1;
    if (res) {
        LOG_INFO("%s: riot_hdr checksum invalid\n", __func__);
    }

    return res;
}

uint32_t riot_hdr_checksum(const riot_hdr_t *riot_hdr)
{
    return fletcher32((uint16_t*)riot_hdr, offsetof(riot_hdr_t, chksum) / sizeof(uint16_t));
}
