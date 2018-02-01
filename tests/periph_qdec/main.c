/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
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
    puts("Welcome into QDEC test program !");

    for (i = 0; i < QDEC_NUMOF; i++)
    {
        error = qdec_init(QDEC_DEV(i), QDEC_X4, handler, NULL);
        if (error)
        {
            fprintf(stderr,"Not supported mode !\n");
            return error;
        }
    }

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    while(1) {
        for (i = 0; i < QDEC_NUMOF; i++)
        {
            printf("QDEC %u = %ld\n", i, qdec_read(QDEC_DEV(i)));
        }
        xtimer_sleep(1);
    }

    return 0;
}
