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
__attribute__((used,section(".puf_stack"))) uint32_t puf_sram_seed;

/* Allocation of the PUF seed state */
__attribute__((used,section(".puf_stack"))) uint32_t puf_sram_state;

/* Allocation of the memory marker */
__attribute__((used,section(".puf_stack"))) uint32_t puf_sram_marker;

uint32_t puf_sram_generate(const uint8_t *ram, size_t len)
{
    /* build hash from start-up pattern */
    puf_sram_seed = dek_hash(ram, len);
    /* write marker to a defined section for subsequent reset detection */
    puf_sram_marker = PUF_SRAM_MARKER;
    /* seting state to 0 means seed was generated from SRAM pattern */
    puf_sram_state = 0;
    return puf_sram_seed;
}

bool puf_sram_softreset(void)
{
    if(puf_sram_marker != PUF_SRAM_MARKER){
        puf_sram_state = 2;
        return 0;
    }
    puf_sram_state = 1;
    return 1;
}
