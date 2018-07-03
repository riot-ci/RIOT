/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Implementation of the kernels irq interface
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG    0
#include "debug.h"

#include <stdint.h>
#include <stdio.h>

#include "irq.h"
#include "cpu.h"

#include "common.h"
#include "esp/common_macros.h"
#include "esp/xtensa_ops.h"
#include "rom/ets_sys.h"
#include "xtensa/xtensa_context.h"

extern unsigned _xtos_set_intlevel(unsigned intlevel);

/**
 * @brief Set on entry into and reset on exit from an ISR
 */
volatile uint32_t irq_interrupt_nesting = 0;

/**
 * @brief Disable all maskable interrupts
 */
unsigned int IRAM irq_disable(void)
{
    uint32_t _saved_intlevel;

    /* read and set interrupt level with one asm instruction (RSIL) */
    __asm__ volatile ("rsil %0, " XTSTR(XCHAL_NUM_INTLEVELS+1) : "=a" (_saved_intlevel));
    /* mask out everything else of the PS register that do not belong to
       interrupt level (bits 3..0) */
    _saved_intlevel &= 0xf;

    DEBUG ("%s new %08lx (old %08lx)\n", __func__,
           XCHAL_NUM_INTLEVELS + 1, _saved_intlevel);
    return _saved_intlevel;
}

/**
 * @brief Enable all maskable interrupts
 */
unsigned int IRAM irq_enable(void)
{
    uint32_t _saved_intlevel;

    /* read and set interrupt level with one asm instruction (RSIL) */
    __asm__ volatile ("rsil %0, 0" : "=a" (_saved_intlevel));
    /* mask out everything else of the PS register that do not belong to
       interrupt level (bits 3..0) */
    _saved_intlevel &= 0xf;

    DEBUG ("%s new %08lx (old %08lx)\n", __func__, 0, _saved_intlevel);
    return _saved_intlevel;
}

/**
 * @brief Restore the state of the IRQ flags
 */
void IRAM irq_restore(unsigned int state)
{
    /* restore the interrupt level using a rom function, performance is not
       important here */
    #if 0
    __asm__ volatile ("wsr %0, ps; rsync" :: "a" (state));
    DEBUG ("%s %02x\n", __func__, state);
    #else
    unsigned _saved_intlevel = _xtos_set_intlevel(state);
    DEBUG ("%s new %08lx (old %08lx)\n", __func__, state, _saved_intlevel);
    #endif
}

/**
 * @brief See if the current context is inside an ISR
 */
int IRAM irq_is_in(void)
{
    DEBUG("irq_interrupt_nesting = %d\n", irq_interrupt_nesting);
    return irq_interrupt_nesting;
}
