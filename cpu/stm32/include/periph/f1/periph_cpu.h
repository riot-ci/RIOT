/*
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32f1
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_F1_PERIPH_CPU_H
#define PERIPH_F1_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Starting address of the CPU ID
 */
#define CPUID_ADDR          (0x1ffff7e8)

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_IRQ_PRIO        1

#define RTT_DEV             RTC
#define RTT_IRQ             RTC_IRQn
#define RTT_ISR             isr_rtc

#define RTT_MAX_VALUE       (0xffffffff)
#define RTT_CLOCK_FREQUENCY (32768U)                  /* in Hz */
#define RTT_MIN_FREQUENCY   (1U)                      /* in Hz */
/* RTC frequency of 32kHz is not recommended, see RM0008 Rev 20, p490 */
#define RTT_MAX_FREQUENCY   (RTT_CLOCK_FREQUENCY / 2) /* in Hz */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_F1_PERIPH_CPU_H */
/** @} */
