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

#define QDEC_ID 0

void handler(void *arg)
{
    (void)arg;
    printf("INTERRUPT\n");
}

int main(void)
{
    puts("Welcome into QDEC test program !");
    qdec_init(QDEC_ID, QDEC_X4, handler, NULL);

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    while(1) {
        printf("QDEC %u = %ld\n", QDEC_ID, qdec_read(QDEC_ID));
        xtimer_sleep(1);
    }

    return 0;
}
