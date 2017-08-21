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
#ifndef CLOCK_CORECLOCK
#error "Please provide desired CLOCK_CORECLOCK in your board's periph_conf.h"
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
#define PLL_IN                      (16000000U)         /* HSI fixed @ 16MHz */
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif
#if (CLOCK_ENABLE_PLLI2S)
#ifndef CLOCK_I2S
#define CLOCK_I2S                   (48000000U)         /* Default value 48MHz */
#endif
#endif
#if (CLOCK_ENABLE_PLLSAI)
#ifndef CLOCK_SAI
#define CLOCK_SAI                   (48000000U)         /* Default value 48MHz */
#endif
#endif

#include "clk/pll.h"
#include "clk/plli2s.h"
#include "clk/pllsai.h"

#if (CLOCK_ENABLE_PLLI2S)
#ifdef RCC_PLLI2SCFGR_PLLI2SM_Pos
#define PLLI2S_M                 (M_I2S << RCC_PLLI2SCFGR_PLLI2SM_Pos)
#else
#define PLLI2S_M                 (0)
#endif
#define PLLI2S_N                 (N_I2S << RCC_PLLI2SCFGR_PLLI2SN_Pos)
#define PLLI2S_Q                 (Q_I2S << RCC_PLLI2SCFGR_PLLI2SQ_Pos)
#endif /* CLOCK_ENABLE_PLLI2S */

#if (CLOCK_ENABLE_PLLSAI)
#define PLLSAI_N                 (N_SAI << RCC_PLLSAICFGR_PLLSAIN_Pos)
#define PLLSAI_Q                 (Q_SAI << RCC_PLLSAICFGR_PLLSAIQ_Pos)
#endif

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
#ifndef RCC_CFGR_MCO2
#error "stmclk: no MCO2 on this device"
#endif
    RCC->CFGR |= CLOCK_MCO2_SRC | CLOCK_MCO2_PRE;
#endif

    /* if configured, we need to enable the HSE clock now */
#if (CLOCK_HSE)
    RCC->CR |= (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

#if CLOCK_48MHZ_2ND_PLL
    RCC->DCKCFGR2 |= RCC_DCKCFGR2_CK48MSEL;
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

#if (CLOCK_ENABLE_PLLSAI)
    RCC->PLLSAICFGR = (PLLSAI_N | PLLSAI_Q);
    RCC->CR |= (RCC_CR_PLLSAION);
    while (!(RCC->CR & RCC_CR_PLLSAIRDY)) {}
#endif

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
