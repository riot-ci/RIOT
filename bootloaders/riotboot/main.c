/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    bootloader Minimal riot-based bootloader
 * @ingroup     bootloader
 * @{
 *
 * @file
 * @brief       RIOT Bootloader
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "panic.h"
#include "riot_hdr.h"
#include "slot_util.h"

void kernel_init(void)
{
    /* read the header for slot 1 (slot 0 is the bootlaoder) */
    riot_hdr_t *slot_hdr = slot_util_get_hdr(1);

    /* if the header is valid, jump to it */
    if (riot_hdr_validate(slot_hdr) == 0) {
        /* jump to slot 1 where the firmware image is */
        slot_util_jump(1);
    }

    /* serious trouble! */
    while (1) {}
}

NORETURN void core_panic(core_panic_t crash_code, const char *message)
{
    (void)crash_code;
    (void)message;
    while (1) {}
}
