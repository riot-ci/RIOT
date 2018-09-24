/*
 * Copyright (c) 2016 Inria
 *               2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 *
 */

/**
 * @file
 * @brief       Header generation tool for RIOT firmware images
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "riot_hdr.h"
#include "common.h"

/**
 *  @brief Alignment required to set VTOR in Cortex-M0+/3/4/7
 */
#define HDR_ALIGN              (256)

const char generate_usage[] = "genhdr generate <IMG_BIN> <APP_VER> <START_ADDR> <HDR_LEN> <outfile|->";

int genhdr(int argc, char *argv[])
{
    /* riot_hdr header and buffer */
    riot_hdr_t riot_hdr;
    uint8_t *hdr_buf;

    /* arguments storage variables */
    long app_ver_arg = 0;
    long start_addr_arg = 0;
    long hdr_len_arg = 0;

    /* header variables */
    int riot_hdr_len = 0;
    uint32_t app_ver = 0;
    uint32_t start_addr = 0;

    /* helpers */
    errno = 0;
    char *p;

    if (argc < 6) {
        fprintf(stderr, "usage: %s\n", generate_usage);
        return -1;
    }

    app_ver_arg = strtol(argv[2], &p, 0);
    if (errno != 0 || *p != '\0' || app_ver_arg > INT_MAX) {
        fprintf(stderr, "Error: APP_VER not valid!\n");
    } else {
        app_ver = app_ver_arg;
    }

    start_addr_arg = strtol(argv[3], &p, 0);
    if (errno != 0 || *p != '\0' || start_addr_arg > INT_MAX) {
        fprintf(stderr, "Error: START_ADDR not valid!\n");
    } else {
        start_addr = start_addr_arg;
    }

    hdr_len_arg = strtol(argv[4], &p, 0);
    if (errno != 0 || *p != '\0' || hdr_len_arg % HDR_ALIGN || hdr_len_arg > INT_MAX) {
        fprintf(stderr, "Error: HDR_LEN not valid!\n");
        return -1;
    }
    else {
        riot_hdr_len = hdr_len_arg;
    }

    /* prepare the buffer for riot_hdr_t */
    hdr_buf = malloc(riot_hdr_len);
    if (hdr_buf == NULL) {
        fprintf(stderr, "Error: not enough memory!\n");
        return -1;
    }

    /* ensure the buffer and header have 0's */
    memset(&riot_hdr, '\0', sizeof(riot_hdr_t));
    memset(hdr_buf, '\0', riot_hdr_len);

    /* Generate image header */
    memcpy(&riot_hdr.magic_number, "RIOT", 4);
    riot_hdr.version = app_ver;
    riot_hdr.start_addr = start_addr;

    /* calculate header checksum */
    riot_hdr.chksum = riot_hdr_checksum(&riot_hdr);

    /* save riot_hdr to buffer */
    memcpy(hdr_buf, &riot_hdr, sizeof(riot_hdr_t));

    /* Write the header */
    if (!to_file(argv[5], hdr_buf, riot_hdr_len)) {
        fprintf(stderr, "Error: cannot write output\n");
        free(hdr_buf);
        return(1);
    }

    free(hdr_buf);

    return 0;
}
