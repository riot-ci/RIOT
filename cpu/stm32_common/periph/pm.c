/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *               2015 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *               2017-2018 OTA keys S.A.
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @ingroup     drivers_periph_pm
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "irq.h"
#include "periph/pm.h"
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32L4)
#include "stmclk.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifndef PM_STOP_CONFIG
/**
 * @brief Define config flags for stop mode
 *
 * Available values can be found in reference manual, PWR section, register CR.
 */
#if defined(CPU_FAM_STM32F0)
#define PM_STOP_CONFIG  (PWR_CR_LPDS)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define PM_STOP_CONFIG  (PWR_CR_LPSDSR | PWR_CR_ULP)
#elif defined(CPU_FAM_STM32L4)
#define PM_STOP_CONFIG  (PWR_CR1_LPMS_STOP1)
#elif defined(CPU_FAM_STM32F7)
#define PM_STOP_CONFIG  (PWR_CR1_LPDS | PWR_CR1_FPDS | PWR_CR1_LPUDS)
#else
#define PM_STOP_CONFIG  (PWR_CR_LPDS | PWR_CR_FPDS)
#endif
#endif

#ifndef PM_STOP_CLEAR
/**
 * @brief Define flags cleared before configuring stop mode
 */
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define PM_STOP_CLEAR   (PWR_CR_LPSDSR | PWR_CR_PDDS)
#elif defined(CPU_FAM_STM32L4)
#define PM_STOP_CLEAR   (PWR_CR1_LPMS)
#elif defined(CPU_FAM_STM32F7)
#define PM_STOP_CLEAR   (PWR_CR1_LPDS | PWR_CR1_PDDS)
#else
#define PM_STOP_CLEAR   (PWR_CR_LPDS | PWR_CR_PDDS)
#endif
#endif

#if defined(CPU_FAM_STM32L4)
#define PWR_CR_REG     PWR->CR1
#define PWR_WUP_REG    PWR->CR3
#elif defined(CPU_FAM_STM32F7)
#define PWR_CR_REG     PWR->CR1
#define PWR_WUP_REG    PWR->CSR2
#else
#define PWR_CR_REG     PWR->CR
#define PWR_WUP_REG    PWR->CSR
#endif

static inline uint32_t _ewup_config(void)
{
    uint32_t tmp = 0;
#ifdef PM_EWUP_CONFIG
    tmp |= PM_EWUP_CONFIG;
#elif defined(PWR_CSR_EWUP)
    tmp |= PWR_CSR_EWUP;
#elif defined(PWR_CR3_EWUP)
    tmp |= PWR_CR3_EWUP;
#else
#if defined(PWR_CSR_EWUP8)
    tmp |= PWR_CSR_EWUP8;
#endif
#if defined(PWR_CSR_EWUP7)
    tmp |= PWR_CSR_EWUP7;
#endif
#if defined(PWR_CSR_EWUP6)
    tmp |= PWR_CSR_EWUP6;
#endif
#if defined(PWR_CSR_EWUP5)
    tmp |= PWR_CSR_EWUP5;
#endif
#if defined(PWR_CSR_EWUP4)
    tmp |= PWR_CSR_EWUP4;
#endif
#if defined(PWR_CSR_EWUP3)
    tmp |= PWR_CSR_EWUP3;
#endif
#if defined(PWR_CSR_EWUP2)
    tmp |= PWR_CSR_EWUP2;
#endif
#if defined(PWR_CSR_EWUP1)
    tmp |= PWR_CSR_EWUP1;
#endif
#endif
    return tmp;
}

void pm_set(unsigned mode)
{
    int deep = 0;

/* I just copied it from stm32f1/2/4, but I suppose it would work for the
 * others... /KS */
#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32L4)
    switch (mode) {
        case STM32_PM_STANDBY:
#if defined(CPU_FAM_STM32L4)
            /* Enable Standby mode */
            PWR->CR1 &= ~PWR_CR1_LPMS;
            PWR->CR1 |= PWR_CR1_LPMS_STANDBY;
            /* Disable SRAM2 retention */
            PWR->CR3 &= ~PWR_CR3_RRS;
            /* Clear flags */
            PWR->SCR |= PWR_SCR_CSBF;
#elif defined(CPU_FAM_STM32F7)
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR1 |= (PWR_CR1_PDDS | PWR_CR1_CSBF);
#else
            /* Set PDDS to enter standby mode on deepsleep and clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
            /* Enable WKUP pin to use for wakeup from standby mode */
#if defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
            /* Enable Ultra Low Power mode */
            PWR->CR |= PWR_CR_ULP;
#endif
#endif /* CPU_FAM_STM32L4 */
            PWR_WUP_REG |= _ewup_config();
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
        case STM32_PM_STOP:
            /* Clear PDDS and LPDS bits to enter stop mode on */
            /* deepsleep with voltage regulator on */
            PWR_CR_REG &= ~(PM_STOP_CLEAR);
            PWR_CR_REG |= PM_STOP_CONFIG;
            /* Set SLEEPDEEP bit of system control block */
            deep = 1;
            break;
    }
#else
    (void) mode;
#endif

    cortexm_sleep(deep);

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32L4)
    if (deep) {
        /* Re-init clock after STOP */
        stmclk_init_sysclk();
    }
#endif
}

#if defined(CPU_FAM_STM32F0) || defined(CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L0) || \
    defined(CPU_FAM_STM32L1) || defined(CPU_FAM_STM32L4)
void pm_off(void)
{
    irq_disable();
    pm_set(0);
}
#endif
