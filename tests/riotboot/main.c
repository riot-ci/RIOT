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

#include "slot_util.h"
#include "riot_hdr.h"

int main(void)
{
    puts("Hello riotboot!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    /* print some information about the running image */
    unsigned current_slot = slot_util_current_slot();
    riot_hdr_t *riot_hdr = slot_util_get_hdr(current_slot);
    printf("riot_hdr: running from slot %u\n", current_slot);
    riot_hdr_print(riot_hdr);

    return 0;
}
