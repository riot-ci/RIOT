/*
 * Copyright (C) 2017 Gilles DOFFE <gdoffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level QDEC drivers
 *
 * This test initializes all declared QDEC devices.
 * It displays QDEC counters value each second.
 *
 * @author      Gilles DOFFE <gdoffe@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "periph/qdec.h"
#include "xtimer.h"

void handler(void *arg)
{
    (void)arg;
    printf("INTERRUPT\n");
}

int main(void)
{
    uint8_t i = 0;
    int32_t error = 0;
    puts("Welcome into Quadrature Decoder (QDEC) test program.");
    puts("This program will count pulses on all available QDEC channels");
    puts("Written for nucleo-f401, you have to plug signals A and B as follow :");
    puts("  QDEC0 : signal A on PB4 and signal B on PC7");
    puts("  QDEC1 : signal A on PB6 and signal B on PB7");
    puts("Quadrature decoding mode is set to X4 : counting on all edges on both signals");

    for (i = 0; i < QDEC_NUMOF; i++) {
        error = qdec_init(QDEC_DEV(i), QDEC_X4, handler, NULL);
        if (error) {
            fprintf(stderr,"Not supported mode !\n");
            return error;
        }
    }

    while(1) {
        for (i = 0; i < QDEC_NUMOF; i++) {
            printf("QDEC %u = %ld\n", i, qdec_read(QDEC_DEV(i)));
        }
        xtimer_sleep(1);
    }

    return 0;
}
