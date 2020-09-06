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

/* I2S clock source */
#ifndef CONFIG_PLLI2S_SRC
#define CONFIG_PLLI2S_SRC           (0)     /* PLLI2S used as I2S clock source */
#else
#define CONFIG_PLLI2S_SRC           (1)     /* Use external I2S source */
#endif

/* Compute the bitfields for the PLL configuration */
#define PLL_M                       (CONFIG_CLOCK_PLL_M << RCC_PLLCFGR_PLLM_Pos)
#define PLL_N                       (CONFIG_CLOCK_PLL_N << RCC_PLLCFGR_PLLN_Pos)
#define PLL_P                       (((CONFIG_CLOCK_PLL_P / 2) - 1) << RCC_PLLCFGR_PLLP_Pos)
#define PLL_Q                       (CONFIG_CLOCK_PLL_Q << RCC_PLLCFGR_PLLQ_Pos)
#if defined(RCC_PLLCFGR_PLLR_Pos)
#define PLL_R                       (CONFIG_CLOCK_PLL_R << RCC_PLLCFGR_PLLR_Pos)
#else
#define PLL_R                       (0)
#endif

/* Select 48MHz clock source between PLLQ, PLLI2SQ or PLLSAIQ. This depends on
   the PLL parameters and if not possible on CPU lines which can provide 48MHz
   from PLLI2S or PLLSAI */

/* Compute the clock frequency output PLLQ */
#define CLOCK_PLLQ                  (((CLOCK_PLL_SRC / CONFIG_CLOCK_PLL_M) * CONFIG_CLOCK_PLL_N) / CONFIG_CLOCK_PLL_Q)

/* Determine if PLL is required, even if not used as SYSCLK
   This is the case when USB is used in application and PLLQ is configured to
   output 48MHz */
#if IS_USED(MODULE_PERIPH_USBDEV) && (CLOCK_PLLQ == MHZ(48))
#define CONFIG_CLOCK_REQUIRE_PLLQ    1
#else
#define CONFIG_CLOCK_REQUIRE_PLLQ    0
#endif

/* PLLI2S can only be used for USB with F412/F413/F423 lines
   PLLI2S is only enabled if no suitable 48MHz clock source can be generated with PLLQ */
#if (defined(CPU_LINE_STM32F412Cx) || defined(CPU_LINE_STM32F412Rx) || \
     defined(CPU_LINE_STM32F412Vx) || defined(CPU_LINE_STM32F412Zx) || \
     defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx)) && \
    IS_USED(MODULE_PERIPH_USBDEV) && !IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLQ)
#define CONFIG_CLOCK_REQUIRE_PLLI2SR 1
#else
/* Disable PLLI2S if USB is not required or is required but PLLQ cannot generate 48MHz clock */
#define CONFIG_CLOCK_REQUIRE_PLLI2SR 0
#endif

/* PLLSAI can only be used for USB with F446/469/479 lines and F7
   PLLSAI is only enabled if no suitable 48MHz clock source can be generated with PLLQ */
#if (defined(CPU_LINE_STM32F446xx) || defined(CPU_LINE_STM32F469xx) || \
     defined(CPU_LINE_STM32F479xx) || defined(CPU_FAM_STM32F7)) && \
    IS_USED(MODULE_PERIPH_USBDEV) && !IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLQ)
#define CONFIG_CLOCK_REQUIRE_PLLSAIP  1
#else
/* Disable PLLSAI if USB is not required or is required but PLLQ cannot generate 48MHz clock */
#define CONFIG_CLOCK_REQUIRE_PLLSAIP  0
#endif

#if IS_USED(MODULE_PERIPH_USBDEV) && \
    !(IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLQ) || \
      IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLI2SR) || \
      IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLSAIP))
#error No suitable 48MHz found, USB will not work
#endif

/* PLLI2S configuration: the following parameters configure a 48MHz I2S clock
   with HSE (8MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLLI2S_M
/* PLLM factor is not shared with PLLI2S on F412/413/423/446 cpu lines */
#if defined(CPU_LINE_STM32F412Cx) || defined(CPU_LINE_STM32F412Rx) || \
    defined(CPU_LINE_STM32F412Vx) || defined(CPU_LINE_STM32F412Zx) || \
    defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx) || \
    defined(CPU_LINE_STM32F446xx)
#define CONFIG_CLOCK_PLLI2S_M       (4)
#else
#define CONFIG_CLOCK_PLLI2S_M       CONFIG_CLOCK_PLL_M
#endif
#endif
#ifndef CONFIG_CLOCK_PLLI2S_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CONFIG_CLOCK_PLLI2S_N       (192)
#else
#define CONFIG_CLOCK_PLLI2S_N       (96)
#endif
#endif
#ifndef CONFIG_CLOCK_PLLI2S_P
#define CONFIG_CLOCK_PLLI2S_P       (8)     /* SPDIF-Rx clock, 48MHz by default */
#endif
#ifndef CONFIG_CLOCK_PLLI2S_Q
#define CONFIG_CLOCK_PLLI2S_Q       (8)     /* Alternative 48MHz clock (USB) and/or MCO2 PLLI2S */
#endif
#ifndef CONFIG_CLOCK_PLLI2S_R
#define CONFIG_CLOCK_PLLI2S_R       (8)     /* I2S clock, 48MHz by default */
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
#if defined(RCC_PLLI2SCFGR_PLLI2SQ_Pos)
#define PLLI2S_Q                    (CONFIG_CLOCK_PLLI2S_Q << RCC_PLLI2SCFGR_PLLI2SQ_Pos)
#else
#define PLLI2S_Q                    (0)
#endif
#if defined(RCC_PLLI2SCFGR_PLLI2SR_Pos)
#define PLLI2S_R                    (CONFIG_CLOCK_PLLI2S_R << RCC_PLLI2SCFGR_PLLI2SR_Pos)
#else
#define PLLI2S_R                    (0)
#endif

/* PLLSAI configuration: the following parameters configure a 48MHz SAI clock
   with HSE (8MHz) or HSI (16MHz) as PLL input clock */
#ifndef CONFIG_CLOCK_PLLSAI_M
/* PLLM factor is not shared with PLLSAI on F412/413/423/446 cpu lines */
#if defined(CPU_LINE_STM32F412Cx) || defined(CPU_LINE_STM32F412Rx) || \
    defined(CPU_LINE_STM32F412Vx) || defined(CPU_LINE_STM32F412Zx) || \
    defined(CPU_LINE_STM32F413xx) || defined(CPU_LINE_STM32F423xx) || \
    defined(CPU_LINE_STM32F446xx)
#define CONFIG_CLOCK_PLLSAI_M       (4)
#else
#define CONFIG_CLOCK_PLLSAI_M       CONFIG_CLOCK_PLL_M
#endif
#endif
#ifndef CONFIG_CLOCK_PLLSAI_N
#if IS_ACTIVE(CONFIG_BOARD_HAS_HSE)
#define CONFIG_CLOCK_PLLSAI_N       (192)
#else
#define CONFIG_CLOCK_PLLSAI_N       (96)
#endif
#endif
#ifndef CONFIG_CLOCK_PLLSAI_P
#define CONFIG_CLOCK_PLLSAI_P       (8)     /* Alternative 48MHz clock (USB) */
#endif
#ifndef CONFIG_CLOCK_PLLSAI_Q
#define CONFIG_CLOCK_PLLSAI_Q       (8)     /* SAI clock, 48MHz by default */
#endif
#ifndef CONFIG_CLOCK_PLLSAI_R
#define CONFIG_CLOCK_PLLSAI_R       (8)     /* LCD clock, 48MHz by default */
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

/* Check whether PLL must be enabled */
#ifndef CONFIG_CLOCK_ENABLE_PLL
#if IS_ACTIVE(CONFIG_USE_CLOCK_PLL) || IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLQ) || \
    IS_ACTIVE(CONFIG_CLOCK_MCO1_USE_PLL) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLL)
#define CONFIG_CLOCK_ENABLE_PLL     1
#else
#define CONFIG_CLOCK_ENABLE_PLL     0
#endif
#endif /* CONFIG_CLOCK_ENABLE_PLL */

/* Check whether PLLI2S must be enabled */
#ifndef CONFIG_CLOCK_ENABLE_PLLI2S
#if IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLI2SR) || IS_ACTIVE(CONFIG_CLOCK_MCO2_USE_PLLI2S)
#define CONFIG_CLOCK_ENABLE_PLLI2S     1
#else
#define CONFIG_CLOCK_ENABLE_PLLI2S     0
#endif
#endif /* CONFIG_CLOCK_ENABLE_PLLI2S */

/* Check whether PLLSAI must be enabled */
#ifndef CONFIG_CLOCK_ENABLE_PLLSAI
#if IS_ACTIVE(CONFIG_CLOCK_REQUIRE_PLLSAIP)
#define CONFIG_CLOCK_ENABLE_PLLSAI     1
#else
#define CONFIG_CLOCK_ENABLE_PLLSAI     0
#endif
#endif /* CONFIG_CLOCK_ENABLE_PLLSAI */

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

    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLL)) {
        /* if configured, we need to enable the HSE clock now */
        if (IS_ACTIVE(CONFIG_BOARD_HAS_HSE)) {
            RCC->CR |= (RCC_CR_HSEON);
            while (!(RCC->CR & RCC_CR_HSERDY)) {}
        }

        /* now we can safely configure and start the PLL */
        RCC->PLLCFGR = (PLL_SRC | PLL_M | PLL_N | PLL_P | PLL_Q | PLL_R);
        RCC->CR |= (RCC_CR_PLLON);
        while (!(RCC->CR & RCC_CR_PLLRDY)) {}

        if (IS_ACTIVE(CONFIG_USE_CLOCK_PLL)) {
            /* now that the PLL is running, we use it as system clock */
            RCC->CFGR |= (RCC_CFGR_SW_PLL);
            while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
        }
    }

    if (IS_ACTIVE(CONFIG_USE_CLOCK_HSE) ||
        (IS_ACTIVE(CONFIG_USE_CLOCK_PLL) && IS_ACTIVE(CONFIG_BOARD_HAS_HSE))) {
        /* Disable HSI only if not used */
        stmclk_disable_hsi();
    }

#if defined(RCC_CR_PLLI2SON)
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLI2S)) {
        RCC->PLLI2SCFGR = (CONFIG_PLLI2S_SRC | PLLI2S_M | PLLI2S_N | PLLI2S_P | PLLI2S_Q | PLLI2S_R);
        RCC->CR |= (RCC_CR_PLLI2SON);
        while (!(RCC->CR & RCC_CR_PLLI2SRDY)) {}
    }
#endif

#if defined(RCC_CR_PLLSAION)
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLL_SAI)) {
        RCC->PLLSAICFGR = (PLLSAI_M | PLLSAI_N | PLLSAI_P | PLLSAI_Q | PLLSAI_R);
        RCC->CR |= (RCC_CR_PLLSAION);
        while (!(RCC->CR & RCC_CR_PLLSAIRDY)) {}
    }
#endif

#if defined(RCC_DCKCFGR2_CK48MSEL)
    if (IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLI2S) || IS_ACTIVE(CONFIG_CLOCK_ENABLE_PLLSAI)) {
        /* Use PLLSAI_P or PLLI2S_Q clock source */
        RCC->DCKCFGR2 |= RCC_DCKCFGR2_CK48MSEL;
    }
#endif

    irq_restore(is);
}
