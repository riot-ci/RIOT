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

#include "xtimer.h"
#include "log.h"


int main(void)
{
    xtimer_sleep(1);
    LOG_ERROR("Hello World!\n");
    LOG_WARNING("Hello World!\n");
    LOG_DEBUG("Hello World!\n");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    LOG_WARNING("This board features a(n) %s MCU.\n", RIOT_MCU);

    return 0;
}
