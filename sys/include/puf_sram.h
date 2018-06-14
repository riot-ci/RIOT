/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_puf_sram SRAM PUF
 * @ingroup     sys
 *
 * @brief       SRAM based physically unclonable function (PUF)
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */
#ifndef PUF_SRAM_H
#define PUF_SRAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "hashes.h"
#include "thread.h"

/**
 * @brief SRAM length considered for seeding
 */
#ifndef SEED_RAM_LEN
#define SEED_RAM_LEN     (2048 / sizeof(uint32_t))
#endif

/**
 * @brief Global seed variable, allocated on the stack in puf_sram.c
 */
extern uint32_t global_puf_seed;

/**
 * @brief Global seed state, allocated on the stack in puf_sram.c
 */
extern uint32_t global_puf_state;

/**
 * @brief builds hash from @p SEED_RAM_LEN bytes uninitialized SRAM, writs it
 *        to the global variable @p global_puf_seed and returns the value
 *
 * @param[in] ram pointer to SRAM memory
 *
 * @return  a random number
 */
uint32_t puf_sram_uint32(const uint8_t *ram);

/**
 * @brief checks for the stack marker in the idle thread to determine whether memory
          contains old data. Otherwise it assumes a reboot from power down mode
 *
 * @param[in] ram    pointer to memory position of stack marker in @p idle_stack
 * @param[in] marker pointer to  stack marker
 * @param[in] length length of the stack marker
 *
 * @return    0 when seed was generated from SRAM pattern
 * @return    1 when reset without power down was detected
 */
bool puf_sram_softreset(const uint8_t *ram, const uint8_t *marker, size_t length);


#ifdef __cplusplus
}
#endif
/** @} */
#endif /* PUF_SRAM_H */
