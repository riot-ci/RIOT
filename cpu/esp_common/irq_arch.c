/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG    (0)
#include "debug.h"

#include <stdint.h>
#include <stdio.h>

#include "irq.h"
#include "cpu.h"

#include "esp_common.h"
#include "esp/common_macros.h"
#include "esp/xtensa_ops.h"
#include "xtensa/xtensa_context.h"

/**
 * @brief Set on entry into and reset on exit from an ISR
 */
volatile uint32_t irq_interrupt_nesting = 0;

/**
 * @brief Disable all maskable interrupts
 */
unsigned int IRAM irq_disable(void)
{
    uint32_t state;

    /* read and set interrupt level (RSIL) */
    __asm__ volatile ("rsil %0, " XTSTR(XCHAL_EXCM_LEVEL) : "=a" (state) :: "memory");
    DEBUG ("%s %02x(%02x)\n", __func__,
           (state & 0xfffffff0) | (XCHAL_EXCM_LEVEL), state);
    return state;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int IRAM irq_enable(void)
{
    uint32_t state;

    /* read and set interrupt level (RSIL) */
    __asm__ volatile ("rsil %0, 0" : "=a" (state) :: "memory");
    DEBUG ("%s %02x(%02x)\n", __func__, state & 0xfffffff0, state);
    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void IRAM irq_restore(unsigned int state)
{
    /* write interrupt level and sync */
    DEBUG ("%s %02x\n", __func__, state);
    __asm__ volatile ("wsr %0, ps; rsync" :: "a" (state) : "memory");
}

/**
 * @brief See if the current context is inside an ISR
 */
int IRAM irq_is_in(void)
{
    DEBUG("irq_interrupt_nesting = %d\n", irq_interrupt_nesting);
    return irq_interrupt_nesting;
}
