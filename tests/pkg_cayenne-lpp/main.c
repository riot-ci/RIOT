/*
 * Copyright (C) 2018 Inria
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
 * @brief       Cayenne Low Power Payload example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "cayenne_lpp.h"

#define TEST_BUFFER1 { 0x03, 0x67, 0x01, 0x10, 0x05, 0x67, 0x00, 0xff }
#ifdef BOARD_NATIVE
#define TEST_BUFFER2 { 0x01, 0x67, 0xFF, 0xD8, \
                       0x06, 0x71, 0x04, 0xD1, 0xFB, 0x2F, 0x00, 0x00 }
#else
#define TEST_BUFFER2 { 0x01, 0x67, 0xFF, 0xD7, \
                       0x06, 0x71, 0x04, 0xD2, 0xFB, 0x2E, 0x00, 0x00 }
#endif
#define TEST_BUFFER3 { 0x01, 0x88, 0x06, 0x76, \
                       0x5E, 0xF2, 0x96, 0x0A, 0x00, 0x03, 0xE8 }

static cayenne_lpp_t lpp;

static const uint8_t test_buffer1[] = TEST_BUFFER1;
static const uint8_t test_buffer2[] = TEST_BUFFER2;
static const uint8_t test_buffer3[] = TEST_BUFFER3;

static void _print_buffer(cayenne_lpp_t *lpp)
{
    for (uint8_t i = 0; i < lpp->cursor; ++i) {
        printf("%02X", lpp->buffer[i]);
    }
    puts("");
}

int main(void)
{
    puts("Cayenne LPP test application");

    /* generate payload like the one given as example at
     * https://mydevices.com/cayenne/docs_stage/lora/#lora-cayenne-low-power-payload
     */
    /* Device with 2 temperature sensors */
    cayenne_lpp_add_temperature(&lpp, 3, 27.2);
    cayenne_lpp_add_temperature(&lpp, 5, 25.5);
    _print_buffer(&lpp);
    /* check buffer */
    printf("Test 1: ");
    if (memcmp(lpp.buffer, test_buffer1, lpp.cursor) != 0) {
        puts("FAILED");
        return 1;
    }
    puts("SUCCESS");

    /* Device with temperature and acceleration sensors */
    cayenne_lpp_reset(&lpp);
    cayenne_lpp_add_temperature(&lpp, 1, -4.1);
    cayenne_lpp_add_accelerometer(&lpp, 6, 1.234, -1.234, 0);
    _print_buffer(&lpp);
    /* check buffer */
    printf("Test 2: ");
    if (memcmp(lpp.buffer, test_buffer2, lpp.cursor) != 0) {
        puts("FAILED");
        return 1;
    }
    puts("SUCCESS");

    /* Device with GPS */
    cayenne_lpp_reset(&lpp);
    cayenne_lpp_add_gps(&lpp, 1, 42.3519, -87.9094, 10);
    _print_buffer(&lpp);
    /* check buffer */
    printf("Test 3: ");
    if (memcmp(lpp.buffer, test_buffer3, lpp.cursor) != 0) {
        puts("FAILED");
        return 1;
    }
    puts("SUCCESS");

    return 0;
}
