/*
 * Copyright (C) 2017 Ken Rabold
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_fe310
 * @{
 *
 * @file
 * @brief           Implementation of the kernels irq interface
 *
 * @author          Ken Rabold
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef IRQ_ARCH_H
#define IRQ_ARCH_H

#include <stdint.h>
#include "irq.h"
#include "cpu_conf.h"
#include "cpu.h"

#include "vendor/encoding.h"

#ifdef __cplusplus
extern "C" {
#endif

extern volatile int fe310_in_isr;

/**
 * @brief Enable all maskable interrupts
 */
static inline __attribute__((always_inline)) unsigned int irq_enable(void)
{
    /* Enable all interrupts */
    set_csr(mstatus, MSTATUS_MIE);
    return read_csr(mstatus);
}

/**
 * @brief Disable all maskable interrupts
 */
static inline __attribute__((always_inline)) unsigned int irq_disable(void)
{
    unsigned int state = read_csr(mstatus);

    /* Disable all interrupts */
    clear_csr(mstatus, MSTATUS_MIE);
    return state;
}

/**
 * @brief Restore the state of the IRQ flags
 */
static inline __attribute__((always_inline)) void irq_restore(unsigned int state)
{
    /* Restore all interrupts to given state */
    write_csr(mstatus, state);
}

/**
 * @brief See if the current context is inside an ISR
 */
static inline __attribute__((always_inline)) int irq_is_in(void)
{
    return fe310_in_isr;
}

#ifdef __cplusplus
}
#endif

#endif /* IRQ_ARCH_H */
/** @} */
