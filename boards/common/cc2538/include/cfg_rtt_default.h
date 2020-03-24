/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_cc2538
 * @{
 *
 * @file
 * @brief       Default RTT configuration for cc2538 boards
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef CFG_RTT_DEFAULT_H
#define CFG_RTT_DEFAULT_H

#include <stdint.h>

#include "cpu.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name RTT configuration
 * @{
 */
#define RTT_DEV             SMWDTHROSC
#define RTT_IRQ             SM_TIMER_ALT_IRQn
#define RTT_IRQ_PRIO        1
#define RTT_ISR             isr_sleepmode
#define RTT_MAX_VALUE       (0xffffffff)
#if SYS_CTRL_OSC32K_USE_XTAL
/* Frequency of XOSC off by default */
#define RTT_FREQUENCY       (32768U)    /* in Hz. For changes see `rtt.c` */
#else
/* Frequency of RCOSC on by default */
#define RTT_FREQUENCY       (32753U)    /* in Hz. For changes see `rtt.c` */
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_RTT_DEFAULT_H */
/** @} */
