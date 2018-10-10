/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *                    Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_slot_util
 * @{
 *
 * @file
 * @brief       Slot management functions
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */
#include <string.h>

#include "cpu.h"
#include "slot_util.h"
#include "riot_hdr.h"

/*
 * Store the start addresses of each slot.
 * Take into account that CPU_FLASH_BASE represents the starting
 * address of the bootloader, thus the header is located after the
 * space reserved to the bootloader.
 */
const riot_hdr_t * const slot_util_slots[] = {
    (riot_hdr_t*)(CPU_FLASH_BASE + SLOT0_OFFSET),   /* First slot address -> firmware image */
};

/* Calculate the number of slots */
const unsigned slot_util_num_slots = sizeof(slot_util_slots) / sizeof(uint32_t);

static void _slot_util_jump_to_image(const riot_hdr_t *riot_hdr)
{
    cpu_jump_to_image(riot_hdr->start_addr);
}

int slot_util_current_slot(void)
{
    uint32_t base_addr = cpu_get_image_baseaddr();

    for (unsigned i = 0; i < slot_util_num_slots; i++) {
        const riot_hdr_t *hdr = slot_util_get_hdr(i);
        if (base_addr == hdr->start_addr) {
            return i;
        }
    }

    return -1;
}

void slot_util_jump(unsigned slot)
{
    _slot_util_jump_to_image(slot_util_get_hdr(slot));
}

uint32_t slot_util_get_image_startaddr(unsigned slot)
{
    return slot_util_get_hdr(slot)->start_addr;
}

void slot_util_dump_addrs(void)
{
    for (unsigned slot = 0; slot < slot_util_num_slots; slot++) {
        printf("slot %u: metadata: %p image: 0x%08" PRIx32 "\n", slot,
               slot_util_get_hdr(slot),
               slot_util_get_image_startaddr(slot));
    }
}

const riot_hdr_t *slot_util_get_hdr(unsigned slot)
{
    assert(slot < slot_util_num_slots);

    return slot_util_slots[slot];
}
