/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7)

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* make sure we have all needed information about the clock configuration */
#ifndef CLOCK_HSE
#error "Please provide CLOCK_HSE in your board's perhip_conf.h"
#endif
#ifndef CLOCK_LSE
#error "Please provide CLOCK_LSE in your board's periph_conf.h"
#endif

/**
 * @name    PLL configuration
 * @{
 */
/* figure out which input to use */
#if (CLOCK_HSE)
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      (25000000U)         /* HSI fixed @ 16MHz */
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif
#if (CLOCK_ENABLE_PLLI2S)
#ifndef CLOCK_I2S
#define CLOCK_I2S                   (48000000U)         /* Default value 48MHz */
#endif
#endif

#ifndef P
/* we fix P to 2 (so the PLL output equals 2 * CLOCK_CORECLOCK) */
#define P                       (2U)
#endif /* P */
#if ((P != 2) && (P != 4) && (P != 6) && (P != 8))
#error "PLL configuration: PLL P value is invalid"
#endif
/* the recommended input clock for the PLL should be 2MHz */
#ifndef PLL_IN_FREQ
#define PLL_IN_FREQ             (2000000U)
#endif
#define M                       (PLL_IN / PLL_IN_FREQ)
#if ((M < 2) || (M > 63))
#error "PLL configuration: PLL M value is out of range"
#endif
#if (PLL_IN_FREQ * M) != PLL_IN
#error "PLL configuration: PLL input frequency is invalid (M)"
#endif
/* next we multiply the input freq to P * CORECLOCK */
#define N                       (P * CLOCK_CORECLOCK / PLL_IN_FREQ)
#if ((N < 50) || (N > 432))
#error "PLL configuration: PLL N value is out of range"
#endif
#if (PLL_IN_FREQ * N / P) != CLOCK_CORECLOCK
#error "PLL configuration: PLL input frequency is invalid (N)"
#endif
/* finally we need to set Q, so that the USB clock is 48MHz */
#define Q                       ((P * CLOCK_CORECLOCK) / 48000000U)
#if ((Q * 48000000U) != (P * CLOCK_CORECLOCK))
#ifdef RCC_DCKCFGR2_CK48MSEL           /* cpu support 48MHz clock from PLL I2S */
#if ((CLOCK_ENABLE_PLLI2S) && (CLOCK_I2S != 48000000U))
#error "PLL configuration: PLL I2S should be used for USB, but frequency is not 48MHz"
#else
/* Use PLL I2S @ 48MHz */
#ifndef CLOCK_ENABLE_PLLI2S
#define CLOCK_ENABLE_PLLI2S      (1)
#endif
#ifndef CLOCK_I2S
#define CLOCK_I2S                (48000000U)
#endif
#endif /* ((CLOCK_ENABLE_PLLI2S) && (CLOCK_I2S != 48000000U)) */
#else
#error "PLL configuration: USB frequency is not 48MHz"
#endif /* RCC_DCKCFGR2_CK48MSEL */
#endif

#if (CLOCK_ENABLE_PLLI2S)
#ifndef RCC_CR_PLLI2SRDY
#error "No PLL I2S on this device"
#endif
/* Define CLOCK_I2S_CKIN to use external AFI clock, otherwise, use PLL_IN */
#if (CLOCK_I2S_CKIN)
#define PLLI2S_IN               CLOCK_I2S_CKIN
#define PLLI2S_SRC              RCC_PLLI2SCFGR_PLLI2SSRC
#else
#define PLLI2S_IN               PLL_IN
#define PLLI2S_SRC              0
#endif


#define M_I2S                   (PLLI2S_IN / PLL_IN_FREQ)
#if ((M_I2S < 2) || (M_I2S > 63))
#error "PLL configuration: PLL I2S M value is out of range"
#endif
#if (PLL_IN_FREQ * M_I2S) != PLLI2S_IN
#error "PLL configuration: PLL I2S input frequency is invalid (M)"
#endif

#if Q_I2S
#define N_I2S                   (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if ((N_I2S < 50) || (N_I2S > 432))
#error "PLL configuration: PLL I2S N value is out of range"
#endif
#if (PLL_IN_FREQ * N_I2S / Q_I2S) != CLOCK_I2S
#error "PLL configuration: PLL I2S N value is invalid (try with another Q_I2S)"
#endif
#else

#define Q_I2S  (2U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (3U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (4U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (5U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (6U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (7U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (8U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (9U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (10U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (11U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (12U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (13U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (14U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (15U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if (N_I2S >= 50) && (N_I2S <= 432) && ((PLL_IN_FREQ * N_I2S / Q_I2S) == CLOCK_I2S)
#define _PLL_I2S_FINISHED 1
#endif
#if !_PLL_I2S_FINISHED
#error "PLL configuration: no valid N/Q couple found for PLL I2S configuration"
#endif
#endif /* _PLL_I2S_FINISHED */

#endif /* Q_I2S */

#define PLLI2S_M                 (M_I2S << RCC_PLLI2SCFGR_PLLI2SM_Pos)
#define PLLI2S_N                 (N_I2S << RCC_PLLI2SCFGR_PLLI2SN_Pos)
#define PLLI2S_Q                 (Q_I2S << RCC_PLLI2SCFGR_PLLI2SQ_Pos)

#endif /* CLOCK_ENABLE_PLLI2S */

#if defined(CPU_FAM_STM32F2)
#define RCC_PLLCFGR_PLLP_Pos    (16U)
#define RCC_PLLCFGR_PLLM_Pos    (0U)
#define RCC_PLLCFGR_PLLN_Pos    (6U)
#define RCC_PLLCFGR_PLLQ_Pos    (24U)
#endif

/* now we get the actual bitfields */
#define PLL_P                   (((P / 2) - 1) << RCC_PLLCFGR_PLLP_Pos)
#define PLL_M                   (M << RCC_PLLCFGR_PLLM_Pos)
#define PLL_N                   (N << RCC_PLLCFGR_PLLN_Pos)
#define PLL_Q                   (Q << RCC_PLLCFGR_PLLQ_Pos)
/** @} */

/**
 * @name    Deduct the needed flash wait states from the core clock frequency
 * @{
 */
#define FLASH_WAITSTATES        (CLOCK_CORECLOCK / 30000000U)
/* we enable I+D cashes, pre-fetch, and we set the actual number of
 * needed flash wait states */
#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
#define FLASH_ACR_CONFIG        (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#elif defined(CPU_FAM_STM32F7)
#define FLASH_ACR_CONFIG        (FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#endif
/** @} */

void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    RCC->CIR = 0;

    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV |
                 CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* Flash config */
    FLASH->ACR = FLASH_ACR_CONFIG;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSITRIM_4);

#if (CLOCK_MCO1_SRC)
#ifndef RCC_CFGR_MCO1
#error "stmclk: no MCO1 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO1_SRC | CLOCK_MCO1_PRE;
#endif
#if (CLOCK_MCO2_SRC)
#ifndef RCC_CFGR_MCO1
#error "stmclk: no MCO2 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO2_SRC | CLOCK_MCO2_PRE;
#endif

    /* if configured, we need to enable the HSE clock now */
#if (CLOCK_HSE)
    RCC->CR |= (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

    /* now we can safely configure and start the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_P | PLL_Q);
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= (RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}

    stmclk_disable_hsi();

#if (CLOCK_ENABLE_PLLI2S)
    RCC->PLLI2SCFGR = (PLLI2S_SRC | PLLI2S_M | PLLI2S_N | PLLI2S_Q);
    RCC->CR |= (RCC_CR_PLLI2SON);
    while (!(RCC->CR & RCC_CR_PLLI2SRDY)) {}
#endif /* CLOCK_ENABLE_PLLI2S */

    irq_restore(is);
}

void stmclk_enable_hsi(void)
{
    RCC->CR |= (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {}
}

void stmclk_disable_hsi(void)
{
    if ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {
        RCC->CR &= ~(RCC_CR_HSION);
    }
}

void stmclk_enable_lfclk(void)
{
    /* configure the low speed clock domain (LSE vs LSI) */
#if CLOCK_LSE
    /* allow write access to backup domain */
    stmclk_bdp_unlock();
    /* enable LSE */
    RCC->BDCR |= RCC_BDCR_LSEON;
    while (!(RCC->BDCR & RCC_BDCR_LSERDY)) {}
    /* disable write access to back domain when done */
    stmclk_bdp_lock();
#else
    RCC->CSR |= RCC_CSR_LSION;
    while (!(RCC->CSR & RCC_CSR_LSIRDY)) {}
#endif
}

void stmclk_disable_lfclk(void)
{
#if CLOCK_LSE
    stmclk_bdp_unlock();
    RCC->BDCR &= ~(RCC_BDCR_LSEON);
    stmclk_bdp_lock();
#else
    RCC->CSR &= ~(RCC_CSR_LSION);
#endif
}

void stmclk_bdp_unlock(void)
{
    periph_clk_en(APB1, RCC_APB1ENR_PWREN);
#if defined(CPU_FAM_STM32F7)
    PWR->CR1 |= PWR_CR1_DBP;
#else
    PWR->CR |= PWR_CR_DBP;
#endif
}

void stmclk_bdp_lock(void)
{
#if defined(CPU_FAM_STM32F7)
    PWR->CR1 &= ~(PWR_CR1_DBP);
#else
    PWR->CR &= ~(PWR_CR_DBP);
#endif
    periph_clk_dis(APB1, RCC_APB1ENR_PWREN);
}
#else
typedef int dont_be_pedantic;
#endif /* defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || defined(CPU_FAM_STM32F7) */
