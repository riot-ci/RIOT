/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       riotboot bootloader test
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "slot_util.h"

int main(void)
{
    int current_slot;

    puts("Hello riotboot!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    /* print some information about the running image */
    current_slot = slot_util_current_slot();
    if (current_slot != -1) {
        printf("riotboot_test: running from slot %d\n", current_slot);
        slot_util_print_slot_hdr(current_slot);
    }
    else {
        printf("[FAILED] You're not running riotboot\n");
    }

    return 0;
}
