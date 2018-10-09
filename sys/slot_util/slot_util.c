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

#ifdef RIOT_VERSION
#ifndef BOARD_NATIVE
/*
 * Store the start addresses of each slot.
 * Take into account that CPU_FLASH_BASE represents the starting
 * address of the bootloader, thus we skip it.
 */
const uint32_t slot_util_slots[] = {
    CPU_FLASH_BASE + RIOTBOOT_LEN,   /* First slot address -> firmware image */
};

/* Calculate the number of slots */
const unsigned slot_util_num_slots = sizeof(slot_util_slots) / sizeof(uint32_t);

static void _assert_slot(unsigned slot)
{
    assert(slot < slot_util_num_slots);
    (void)slot;
}

void slot_util_jump_to_image(riot_hdr_t *riot_hdr)
{
    cpu_jump_to_image(riot_hdr->start_addr);
}

int slot_util_current_slot(void)
{
    uint32_t base_addr = cpu_get_image_baseaddr();

    for (unsigned i = 0; i < slot_util_num_slots; i++) {
        riot_hdr_t *hdr = (riot_hdr_t*)slot_util_slots[i];
        if (base_addr == hdr->start_addr) {
            return i;
        }
    }

    return -1;
}

void slot_util_jump(unsigned slot)
{
    _assert_slot(slot);
    slot_util_jump_to_image(slot_util_get_hdr(slot));
}

uint32_t slot_util_get_image_startaddr(unsigned slot)
{
    _assert_slot(slot);

    riot_hdr_t *riot_hdr = (riot_hdr_t*)slot_util_slots[slot];

    return riot_hdr->start_addr;
}

void slot_util_dump_addrs(void)
{
    for (unsigned i = 0; i < slot_util_num_slots; i++) {
        printf("slot %u: metadata: 0x%08x image: 0x%08x\n", i,
               (unsigned)slot_util_slots[i],
               (unsigned)slot_util_get_image_startaddr(i));
    }
}

riot_hdr_t *slot_util_get_hdr(unsigned slot)
{
    _assert_slot(slot);
    return (riot_hdr_t*)slot_util_slots[slot];
}

#else /* BOARD_NATIVE */
const unsigned slot_util_num_slots = 1;

void slot_util_jump_to_image(riot_hdr_t *riot_hdr)
{
    (void)riot_hdr;
    printf("%s native stub\n", __func__);
}

int slot_util_current_slot(void)
{
    printf("%s native stub\n", __func__);

    return 1;
}

unsigned slot_util_get_image_startaddr(unsigned slot)
{
    (void)slot;
    printf("%s native stub\n", __func__);
    return 0;
}

void slot_util_jump(unsigned slot)
{
    (void)slot;
    printf("%s native stub\n", __func__);
}

void slot_util_dump_addrs(void)
{
    printf("%s native stub\n", __func__);
}

riot_hdr_t *slot_util_get_hdr(unsigned slot)
{
    (void)slot;
    printf("%s native stub\n", __func__);
    return NULL;
}

#endif /* BOARD_NATIVE */
#endif /* RIOT_VERSION */
