/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 OTA keys S.A.
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
 * @brief       Implementation of STM32 clock configuration for F0/F1/F2/F3/F4/F7 families
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#include "cpu.h"
#include "stmclk.h"
#include "periph_conf.h"

/* PLL configuration */
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSE
#else
#define PLL_SRC                     RCC_PLLCFGR_PLLSRC_HSI
#endif

#ifndef CONFIG_CLOCK_I2S_SRC
#if IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLI2S)
#define CONFIG_CLOCK_I2S_SRC        (0)     /* PLLI2S used as I2S clock source */
#else
#define CONFIG_CLOCK_I2S_SRC        (1)
#endif
#endif

#if (CONFIG_CLOCK_I2S_SRC == 0) && !IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLI2S)
#error "PLLI2S selected as I2S clock source but is not enabled"
#endif

#if defined(RCC_PLLI2SCFGR_PLLI2SM_Pos)
#define PLLI2S_M                    (CONFIG_CLOCK_PLLI2S_M << RCC_PLLI2SCFGR_PLLI2SM_Pos)
#else
#define PLLI2S_M                    (0)
#endif
#if defined(RCC_PLLI2SCFGR_PLLI2SN_Pos)
#define PLLI2S_N                    (CONFIG_CLOCK_PLLI2S_N << RCC_PLLI2SCFGR_PLLI2SN_Pos)
#else
#define PLLI2S_N                    (0)
#endif
#if defined(RCC_PLLI2SCFGR_PLLI2SP_Pos)
#define PLLI2S_P                    (((CONFIG_CLOCK_PLLI2S_P >> 1) - 1) << RCC_PLLI2SCFGR_PLLI2SP_Pos)
#else
#define PLLI2S_P                    (0)
#endif
#if defined(RCC_PLLI2SCFGR_PLLI2SQ_Pos) && defined(CONFIG_CLOCK_PLLI2S_Q)
#define PLLI2S_Q                    (CONFIG_CLOCK_PLLI2S_Q << RCC_PLLI2SCFGR_PLLI2SQ_Pos)
#else
#define PLLI2S_Q                    (0)
#endif
#if defined(RCC_PLLI2SCFGR_PLLI2SR_Pos) && defined(CONFIG_CLOCK_PLLI2S_R)
#define PLLI2S_R                    (CONFIG_CLOCK_PLLI2S_R << RCC_PLLI2SCFGR_PLLI2SR_Pos)
#else
#define PLLI2S_R                    (0)
#endif

#if defined(RCC_PLLSAICFGR_PLLSAIM_Pos)
#define PLLSAI_M                    (CONFIG_CLOCK_PLLSAI_M << RCC_PLLSAICFGR_PLLSAIM_Pos)
#else
#define PLLSAI_M                    (0)
#endif
#if defined(RCC_PLLSAICFGR_PLLSAIN_Pos)
#define PLLSAI_N                    (CONFIG_CLOCK_PLLSAI_N << RCC_PLLSAICFGR_PLLSAIN_Pos)
#else
#define PLLSAI_N                    (0)
#endif
#if defined(RCC_PLLSAICFGR_PLLSAIP_Pos)
#define PLLSAI_P                    (((CONFIG_CLOCK_PLLSAI_P >> 1) - 1) << RCC_PLLSAICFGR_PLLSAIP_Pos)
#else
#define PLLSAI_P                    (0)
#endif
#if defined(RCC_PLLSAICFGR_PLLSAIQ_Pos)
#define PLLSAI_Q                    (CONFIG_CLOCK_PLLSAI_Q << RCC_PLLSAICFGR_PLLSAIQ_Pos)
#else
#define PLLSAI_Q                    (0)
#endif
#if defined(RCC_PLLSAICFGR_PLLSAIR_Pos)
#define PLLSAI_R                    (CONFIG_CLOCK_PLLSAI_R << RCC_PLLSAICFGR_PLLSAIR_Pos)
#else
#define PLLSAI_R                    (0)
#endif

/* now we get the actual bitfields */
#define PLL_P                       (((CONFIG_CLOCK_PLL_P / 2) - 1) << RCC_PLLCFGR_PLLP_Pos)
#define PLL_M                       (CONFIG_CLOCK_PLL_M << RCC_PLLCFGR_PLLM_Pos)
#define PLL_N                       (CONFIG_CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)
#define PLL_Q                       (CONFIG_CLOCK_PLL_Q << RCC_PLLCFGR_PLLQ_Pos)
#if defined(RCC_PLLCFGR_PLLR_Pos) && defined(CONFIG_CLOCK_PLL_R)
#define PLL_R                       (CONFIG_CLOCK_PLL_R << RCC_PLLCFGR_PLLR_Pos)
#else
#define PLL_R                       (0)
#endif

/* Configure HLCK and PCLK prescalers */
#define CLOCK_AHB_DIV               (RCC_CFGR_HPRE_DIV1)

#if CONFIG_CLOCK_APB1_DIV == 1
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV1)
#elif CONFIG_CLOCK_APB1_DIV == 2
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV2)
#elif CONFIG_CLOCK_APB1_DIV == 4
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV4)
#elif CONFIG_CLOCK_APB1_DIV == 8
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV8)
#elif CONFIG_CLOCK_APB1_DIV == 16
#define CLOCK_APB1_DIV              (RCC_CFGR_PPRE1_DIV16)
#else
#error "Invalid APB1 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

#if CONFIG_CLOCK_APB2_DIV == 1
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV1)
#elif CONFIG_CLOCK_APB2_DIV == 2
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV2)
#elif CONFIG_CLOCK_APB2_DIV == 4
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV4)
#elif CONFIG_CLOCK_APB2_DIV == 8
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV8)
#elif CONFIG_CLOCK_APB2_DIV == 16
#define CLOCK_APB2_DIV              (RCC_CFGR_PPRE2_DIV16)
#else
#error "Invalid APB2 prescaler value (only 1, 2, 4, 8 and 16 allowed)"
#endif

/* Deduct the needed flash wait states from the core clock frequency */
#define FLASH_WAITSTATES            (CLOCK_CORECLOCK / 30000000U)
/* we enable I+D cashes, pre-fetch, and we set the actual number of
 * needed flash wait states */
#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4)
#define FLASH_ACR_CONFIG            (FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#elif defined(CPU_FAM_STM32F7)
#define FLASH_ACR_CONFIG            (FLASH_ACR_ARTEN | FLASH_ACR_PRFTEN | FLASH_WAITSTATES)
#endif

/* Default is not configure MCO1 */
#ifndef CONFIG_CLOCK_ENABLE_MCO1
#define CONFIG_CLOCK_ENABLE_MCO1        0
#endif

#if !defined(RCC_CFGR_MCO1) && IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1)
#error "stmclk: no MCO1 on this device"
#endif

/* Configure the MCO1 clock source: options are PLL (default), HSE, HSI or LSE */
#ifndef CONFIG_CLOCK_MCO1_USE_PLL
#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE)
#define CONFIG_CLOCK_MCO1_USE_PLL   0
#else
#define CONFIG_CLOCK_MCO1_USE_PLL   1     /* Use PLL by default */
#endif
#endif /* CONFIG_CLOCK_MCO1_USE_PLL */

#ifndef CONFIG_CLOCK_MCO1_USE_HSE
#define CONFIG_CLOCK_MCO1_USE_HSE   0
#endif /* CONFIG_CLOCK_MCO1_USE_HSE */

#ifndef CONFIG_CLOCK_MCO1_USE_HSI
#define CONFIG_CLOCK_MCO1_USE_HSI   0
#endif /* CONFIG_CLOCK_MCO1_USE_HSI */

#ifndef CONFIG_CLOCK_MCO1_USE_LSE
#define CONFIG_CLOCK_MCO1_USE_LSE   0
#endif /* CONFIG_CLOCK_MCO2_USE_LSE */

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE))
#error "Cannot use PLL as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE))
#error "Cannot use HSE as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE))
#error "Cannot use HSI as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL))
#error "Cannot use LSE as MCO1 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL)
#define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_1 | RCC_CFGR_MCO1_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSE)
#define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_HSI)
#define CLOCK_MCO1_SRC                          (0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_LSE)
#define CLOCK_MCO1_SRC                          (RCC_CFGR_MCO1_0)
#else
#error "Invalid MCO1 clock source selection"
#endif

/* Configure the MCO1 prescaler: options are 1 to 5 */
#ifndef CONFIG_CLOCK_MCO1_PRE
#define CONFIG_CLOCK_MCO1_PRE                   (1)
#endif

#if CONFIG_CLOCK_MCO1_PRE == 1
#define CLOCK_MCO1_PRE                          (0)
#elif CONFIG_CLOCK_MCO1_PRE == 2
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO1PRE_2)
#elif CONFIG_CLOCK_MCO1_PRE == 3
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_0)
#elif CONFIG_CLOCK_MCO1_PRE == 4
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_1)
#elif CONFIG_CLOCK_MCO1_PRE == 5
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO1PRE_2 | RCC_CFGR_MCO1PRE_1 | RCC_CFGR_MCO1PRE_0)
#else
#error "Invalid MCO1 prescaler"
#endif

/* Default is not configure MCO2 */
#ifndef CONFIG_CLOCK_ENABLE_MCO1
#define CONFIG_CLOCK_ENABLE_MCO1        0
#endif

#if !defined(RCC_CFGR_MCO2) && IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2)
#error "stmclk: no MCO2 on this device"
#endif

/* Configure the MCO2 clock source: options are PLL (default), HSE, HSI or LSE */
#ifndef CONFIG_CLOCK_MCO2_USE_PLL
#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK)
#define CONFIG_CLOCK_MCO2_USE_PLL               0
#else
#define CONFIG_CLOCK_MCO2_USE_PLL               1     /* Use PLL by default */
#endif
#endif /* CONFIG_CLOCK_MCO2_USE_PLL */

#ifndef CONFIG_CLOCK_MCO2_USE_HSE
#define CONFIG_CLOCK_MCO2_USE_HSE               0
#endif /* CONFIG_CLOCK_MCO2_USE_HSE */

#ifndef CONFIG_CLOCK_MCO2_USE_PLLI2S
#define CONFIG_CLOCK_MCO2_USE_PLLI2S            0
#endif /* CONFIG_CLOCK_MCO2_USE_PLLI2S */

#ifndef CONFIG_CLOCK_MCO2_USE_SYSCLK
#define CONFIG_CLOCK_MCO2_USE_SYSCLK            0
#endif /* CONFIG_CLOCK_MCO2_USE_SYSCLK */

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#error "Cannot use PLL as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#error "Cannot use HSE as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK))
#error "Cannot use PLLI2S as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK) && \
    (IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S) || \
     IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL))
#error "Cannot use SYSCLK as MCO2 clock source with other clock"
#endif

#if IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL)
#define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_1 | RCC_CFGR_MCO2_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_HSE)
#define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_1)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S)
#define CLOCK_MCO2_SRC                          (RCC_CFGR_MCO2_0)
#elif IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_SYSCLK)
#define CLOCK_MCO2_SRC                          (0)
#else
#error "Invalid MCO2 clock source selection"
#endif

/* Configure the MCO2 prescaler: options are 1 to 5 */
#ifndef CONFIG_CLOCK_MCO2_PRE
#define CONFIG_CLOCK_MCO2_PRE                   (1)
#endif

#if CONFIG_CLOCK_MCO2_PRE == 1
#define CLOCK_MCO2_PRE                          (0)
#elif CONFIG_CLOCK_MCO2_PRE == 2
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO2PRE_2)
#elif CONFIG_CLOCK_MCO2_PRE == 3
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_0)
#elif CONFIG_CLOCK_MCO2_PRE == 4
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1)
#elif CONFIG_CLOCK_MCO2_PRE == 5
#define CLOCK_MCO1_PRE                          (RCC_CFGR_MCO2PRE_2 | RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_0)
#else
#error "Invalid MCO1 prescaler"
#endif

#if IS_USED(MODULE_PERIPH_USBDEV) && \
    (!CONFIG_CLOCK_ENABLE_PLLI2S || !CONFIG_CLOCK_ENABLE_PLLSAI)
#error "48MHz clock is required for USB but is not enabled"
#endif

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
    RCC->CFGR = (RCC_CFGR_SW_HSI | CLOCK_AHB_DIV | CLOCK_APB1_DIV | CLOCK_APB2_DIV);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}

    /* Flash config */
    FLASH->ACR = FLASH_ACR_CONFIG;

    /* disable all active clocks except HSI -> resets the clk configuration */
    RCC->CR = (RCC_CR_HSION | RCC_CR_HSITRIM_4);

    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO1)) {
        RCC->CFGR |= CLOCK_MCO1_SRC | CLOCK_MCO1_PRE;
    }

    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_MCO2)) {
        RCC->CFGR |= CLOCK_MCO2_SRC | CLOCK_MCO2_PRE;
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE)) {
        RCC->CR |= (RCC_CR_HSEON);
        while (!(RCC->CR & RCC_CR_HSERDY)) {}

        /* Enable HSE as system clock */
        RCC->CFGR |= (RCC_CFGR_SW_HSE);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSE) {}
    }
    else if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {

        /* if configured, we need to enable the HSE clock now */
        if (IS_ACTIVE(CONFIG_BOARD_HAS_HSE)) {
            RCC->CR |= (RCC_CR_HSEON);
            while (!(RCC->CR & RCC_CR_HSERDY)) {}
        }

        /* now we can safely configure and start the PLL */
        RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_P | PLL_Q | PLL_R);
        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}

        /* now that the PLL is running, we use it as system clock */
        RCC->CFGR |= (RCC_CFGR_SW_PLL);
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) ||
        (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && IS_ACTIVE(CONFIG_BOARD_HAS_HSE))) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

#if defined(RCC_CR_PLLI2SON)
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLI2S)) {
        RCC->PLLI2SCFGR = (CONFIG_CLOCK_I2S_SRC | PLLI2S_M | PLLI2S_N | PLLI2S_P | PLLI2S_Q | PLLI2S_R);
        RCC->CR |= (RCC_CR_PLLI2SON);
        while (!(RCC->CR & RCC_CR_PLLI2SRDY)) {}
    }
#endif

#if defined(RCC_CR_PLLSAION)
    if (IS_ACTIVE(CLOCK_ENABLE_PLL_SAI)) {
        RCC->PLLSAICFGR = (PLLSAI_M | PLLSAI_N | PLLSAI_P | PLLSAI_Q | PLLSAI_R);
        RCC->CR |= (RCC_CR_PLLSAION);
        while (!(RCC->CR & RCC_CR_PLLSAIRDY)) {}
    }
#endif

#if defined(RCC_DCKCFGR2_CK48MSEL)
    if (IS_USED(MODULE_PERIPH_USBDEV)) {
        /* Use PLLSAI_P (PLLI2S_Q depending on the case) clock source */
        RCC->DCKCFGR2 |= RCC_DCKCFGR2_CK48MSEL;
    }
#endif

    irq_restore(is);
}
