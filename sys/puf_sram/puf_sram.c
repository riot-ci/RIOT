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

/*  Allocation of the PUF seed variable */
__attribute__((used,section(".puf_stack"))) uint32_t global_puf_seed;

uint32_t puf_sram_uint32(const uint8_t *ram)
{
    global_puf_seed = dek_hash(ram, SEED_RAM_LEN);
    return global_puf_seed;
}
