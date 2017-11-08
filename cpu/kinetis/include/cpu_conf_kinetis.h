/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 * @brief           CPU specific definitions common to all Kinetis CPUs
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_KINETIS_H
#define CPU_CONF_KINETIS_H

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (NUMBER_OF_INT_VECTORS)
#define CPU_FLASH_BASE                  (0x00000000)
/** @} */

/**
 * @name GPIO pin mux function numbers
 * @{
 */
#define PIN_MUX_FUNCTION_ANALOG 0
#define PIN_MUX_FUNCTION_GPIO 1
/** @} */

/**
 * @name GPIO interrupt flank settings
 * @{
 */
#define PIN_INTERRUPT_RISING 0b1001
#define PIN_INTERRUPT_FALLING 0b1010
#define PIN_INTERRUPT_EDGE 0b1011
/** @} */

/**
 * @name LPTMR module configuration
 * @{
 */
#ifndef KINETIS_LPTMR_HZ
#define KINETIS_LPTMR_HZ 32768u
#endif
/** @} */

/**
 * @name   xtimer configuration
 *
 * Use KINETIS_XTIMER_SOURCE_PIT, KINETIS_LPTMR_HZ to choose which configuration
 * to use.
 * @{
 */
#if KINETIS_XTIMER_SOURCE_PIT
/* PIT xtimer configuration */
#define XTIMER_DEV                  (TIMER_PIT_DEV(0))
#define XTIMER_CHAN                 (0)
/* Default xtimer settings should work on the PIT */
#else
/* LPTMR xtimer configuration */
#define XTIMER_DEV                  (TIMER_LPTMR_DEV(0))
#define XTIMER_CHAN                 (0)
/* LPTMR is 16 bits wide and runs at 32768 Hz (clocked by the RTC module), or
 * 1000 Hz (clocked by the internal low-power oscillator) */
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (5)
#define XTIMER_ISR_BACKOFF          (5)
#define XTIMER_OVERHEAD             (4)
#define XTIMER_HZ                   (KINETIS_LPTMR_HZ)
#endif
/** @} */

/**
 * @name Compatibility definitions between vendor headers
 * @{
 */
/* Different versions of vendor headers use some variations of register names.
 * This section aims to eliminate these differences in the few places where it
 * matters for the RIOT driver implementations.
 */
#if !defined(MCG_C2_RANGE0) && defined(MCG_C2_RANGE)
#define MCG_C2_RANGE0(x) MCG_C2_RANGE(x)
#endif
#if !defined(MCG_C2_RANGE0_MASK) && defined(MCG_C2_RANGE_MASK)
#define MCG_C2_RANGE0_MASK MCG_C2_RANGE_MASK
#endif
#if !defined(MCG_C7_OSCSEL) && defined(MCG_C7_OSCSEL_SHIFT)
#define MCG_C7_OSCSEL(x) (((uint32_t)(x) << MCG_C7_OSCSEL_SHIFT) & MCG_C7_OSCSEL_MASK)
#endif
#if !defined(OSC0) && defined(OSC)
#define OSC0 OSC
#endif
#if !defined(SIM_SCGC5_LPTMR_SHIFT) && defined(SIM_SCGC5_LPTIMER_SHIFT)
#define SIM_SCGC5_LPTMR_SHIFT SIM_SCGC5_LPTIMER_SHIFT
#endif
#if !defined(SIM_SCGC5_LPTMR_MASK) && defined(SIM_SCGC5_LPTIMER_MASK)
#define SIM_SCGC5_LPTMR_MASK SIM_SCGC5_LPTIMER_MASK
#endif
#if !defined(GPIOA_BASE) && defined(PTA_BASE)
#define GPIOA_BASE PTA_BASE
#define GPIOB_BASE PTB_BASE
#define GPIOC_BASE PTC_BASE
#define GPIOD_BASE PTD_BASE
#define GPIOE_BASE PTE_BASE
#define GPIOF_BASE PTF_BASE
#define GPIOG_BASE PTG_BASE
#endif
#if !defined(GPIOA) && defined(PTA)
#define GPIOA PTA
#define GPIOB PTB
#define GPIOC PTC
#define GPIOD PTD
#define GPIOE PTE
#define GPIOF PTF
#define GPIOG PTG
#endif
/** @} */

/**
 * @name Timer hardware information
 * @{
 */
#ifdef SIM_SCGC5_LPTMR_SHIFT
/** Enable LPTMR clock gate */
#define LPTMR_CLKEN()  (bit_set32(&SIM->SCGC5, SIM_SCGC5_LPTMR_SHIFT))
#endif
#ifdef SIM_SCGC6_PIT_SHIFT
/** Enable PIT clock gate */
#define PIT_CLKEN()    (bit_set32(&SIM->SCGC6, SIM_SCGC6_PIT_SHIFT))
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_KINETIS_H */
/** @} */
