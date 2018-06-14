/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_puf_sram
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#include "puf_sram.h"

/* Allocation of the PUF seed variable */
__attribute__((used,section(".puf_stack"))) uint32_t global_puf_seed;

/* Allocation of the PUF seed state */
__attribute__((used,section(".puf_stack"))) uint32_t global_puf_state;

uint32_t puf_sram_uint32(const uint8_t *ram)
{
    /* seting state to 0 means seed was generated from
     * SRAM pattern*/
    global_puf_state = 0;
    global_puf_seed = dek_hash(ram, SEED_RAM_LEN);
    return global_puf_seed;
}

bool puf_sram_softreset(const uint8_t *marker_addr, const uint8_t *marker, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        if (marker_addr[i] != marker[i]) {
            global_puf_state = 2;
            return 0;
        }
    }
    /* seting state to 1 means seed was generated from
     * previous round*/
    global_puf_state = 1;
    return 1;
}
