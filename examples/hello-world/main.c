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
#include <inttypes.h>

#include "board.h"

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    printf("Clock coreclock: %" PRIu32 "\n", (uint32_t)CLOCK_CORECLOCK);
    printf("AHB clock: %" PRIu32 "\n", (uint32_t)CLOCK_AHB);
    printf("APB1 clock: %" PRIu32 "\n", (uint32_t)CLOCK_APB1);
#ifdef CLOCK_APB2
    printf("APB2 clock: %" PRIu32 "\n", (uint32_t)CLOCK_APB2);
#endif
#ifdef CLOCK_PLLQ
    printf("PLLQ clock: %" PRIu32 "\n", (uint32_t)CLOCK_PLLQ);
#endif

    return 0;
}
