/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Implementation of STM32 clock configuration for G0 family
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

#define RCC_CFGR_SW_HSI             (0)
#define RCC_CFGR_SW_HSE             (RCC_CFGR_SW_0)
#define RCC_CFGR_SW_PLL             (RCC_CFGR_SW_1)

#if CLOCK_USE_HSE && CLOCK_HSE == 0
#error "HSE is selected as input clock source but CLOCK_HSE is not set"
#endif

#if CLOCK_USE_PLL
#if (CLOCK_PLL_M < 1 || CLOCK_PLL_M > 8)
#error "PLL configuration: PLL M value is out of range"
#endif
#define PLL_M                       ((CLOCK_PLL_M - 1) << RCC_PLLCFGR_PLLM_Pos)

#if (CLOCK_PLL_N < 8 || CLOCK_PLL_N > 86)
#error "PLL configuration: PLL N value is out of range"
#endif
#define PLL_N                       (CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)

#if (CLOCK_PLL_R < 2 || CLOCK_PLL_R > 8)
#error "PLL configuration: PLL R value is out of range"
#endif
#define PLL_R                       ((CLOCK_PLL_R - 1)<< RCC_PLLCFGR_PLLR_Pos)

#if CLOCK_HSE
#define PLL_IN                      CLOCK_HSE
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_IN                      CLOCK_HSI
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif
#endif /* CLOCK_USE_PLL */

/* Determine the required flash wait states from the core clock frequency */
#if CLOCK_CORECLOCK >= 48000000
#define FLASH_WAITSTATES    (FLASH_ACR_LATENCY_1)   /* 2 wait states */
#elif CLOCK_CORECLOCK >= 24000000
#define FLASH_WAITSTATES    (FLASH_ACR_LATENCY_0)   /* 1 wait states */
#else
#define FLASH_WAITSTATES    (0)                     /* 0 wait states */
#endif


void stmclk_init_sysclk(void)
{
    /* disable any interrupts. Global interrupts could be enabled if this is
     * called from some kind of bootloader...  */
    unsigned is = irq_disable();
    /* enable HSI clock for the duration of initialization */
    stmclk_enable_hsi();

    RCC->CIER = 0;

    /* use HSI as system clock while we do any further configuration and
     * configure the AHB and APB clock dividers as configure by the board */
#if CLOCK_USE_HSE
    RCC->CFGR = (RCC_CFGR_SW_HSE | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
#else /* Use HSI as system clock */
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}
#endif

    /* we enable instruction cache, pre-fetch, and we set the required flash wait states */
    FLASH->ACR |= (FLASH_ACR_ICEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES);

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = RCC_CR_HSION;

#if CLOCK_USE_HSE
    /* if configured, we need to enable the HSE clock now */
    RCC->CR |= (RCC_CR_HSEON);
    while (!(RCC->CR & RCC_CR_HSERDY)) {}
#endif

#if CLOCK_LSE
    stmclk_enable_lfclk();
#endif

#if CLOCK_USE_PLL
    /* now we can safely configure and start the PLL */
    RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_R | RCC_PLLCFGR_PLLREN);
    RCC->CR |= (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {}

    /* now that the PLL is running, we use it as system clock */
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
#endif

    stmclk_disable_hsi();
    irq_restore(is);
}
