/*
 * Copyright (c) 2016 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 *
 */

/**
 * @ingroup     tools
 * @file
 * @brief       Header generation tool for RIOT firmware images
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "riot_hdr.h"
#include "common.h"

const char generate_usage[] = "genhdr generate <FIRMWARE> <VERSION> <START-ADDR> <outfile|->";

int genhdr(int argc, char *argv[])
{
    riot_hdr_t riot_hdr;
    uint8_t buf[RIOT_HDR_SIZE];

    memset(&riot_hdr, '\0', sizeof(riot_hdr_t));
    memset(buf, '\0', RIOT_HDR_SIZE);

    if (argc < 5) {
        fprintf(stderr, "usage: %s\n", generate_usage);
        return -1;
    }

    /* Generate image header */
    memcpy(&riot_hdr.magic_number, "RIOT", 4);
    sscanf(argv[2], "%x", (unsigned int *)&(riot_hdr.version));
    sscanf(argv[3], "%u", &(riot_hdr.start_addr));

    /* calculate header checksum */
    riot_hdr.chksum = riot_hdr_checksum(&riot_hdr);

    /* save to buffer of RIOT_HDR_SIZE len */
    memcpy(buf, &riot_hdr, sizeof(riot_hdr_t));

    /* talk to the user */
    if (strcmp(argv[4], "-")) {
        riot_hdr_print(&riot_hdr);
        printf("RIOT header size: %lu\n", sizeof(riot_hdr_t));
    }

    /* Write the header */
    if (!to_file(argv[4], buf, RIOT_HDR_SIZE)) {
        fprintf(stderr, "Error: cannot write output\n");
        return(1);
    }

    return 0;
}
