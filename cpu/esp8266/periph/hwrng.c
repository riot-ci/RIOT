/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Low-level random number generator driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/hwrng.h"

#include "esp/wdev_regs.h"

void hwrng_init(void)
{
    /* no need for initialization */
}

void hwrng_read(void *buf, unsigned int num)
{
    unsigned int count = 0;
    uint8_t *b = (uint8_t *)buf;

    while (count < num) {
        /* read next 4 bytes of random data */
        uint32_t tmp = WDEV.HWRNG;

        /* copy data into result vector */
        for (int i = 0; i < 4 && count < num; i++) {
            b[count++] = (uint8_t)tmp;
            tmp = tmp >> 8;
        }
    }
}

uint32_t hwrng_uint32(void)
{
    return WDEV.HWRNG;
}
