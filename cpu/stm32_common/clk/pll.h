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
 * @brief       STM32 main PLL configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifndef PLL_H
#define PLL_H

#ifdef __cplusplus
extern "C" {
#endif

/* the recommended input clock for the PLL should be 2MHz */
#ifndef PLL_IN_FREQ
#if ((PLL_IN / 2) * 2 == PLL_IN)
#define PLL_IN_FREQ              (2000000U)
#else
#define PLL_IN_FREQ              (1000000U)
#endif
#endif
#define M                        (PLL_IN / PLL_IN_FREQ)
#if ((M < 2) || (M > 63))
#error "PLL configuration: PLL M value is out of range"
#endif
#if (PLL_IN_FREQ * M) != PLL_IN
#error "PLL configuration: PLL input frequency is invalid (M)"
#endif

/* Use user-provided P if existing, otherwise try possible values */
#ifdef P
#if ((P != 2) && (P != 4) && (P != 6) && (P != 8))
#error "PLL configuration: PLL P value is invalid"
#endif
#define N                       (P * CLOCK_CORECLOCK / PLL_IN_FREQ)
#if ((N < 50) || (N > 432))
#error "PLL configuration: PLL N value is out of range"
#endif
#if (PLL_IN_FREQ * N / P) != CLOCK_CORECLOCK
#error "PLL configuration: PLL input frequency is invalid (N)"
#endif
#else
#define P  (2U)
#define N  (P * CLOCK_CORECLOCK / PLL_IN_FREQ)
#if (N >= 50) && (N <= 432) && ((PLL_IN_FREQ * N / P) == CLOCK_CORECLOCK)
#define _PLL_N_FINISHED 1
#endif
#if !_PLL_N_FINISHED
#undef P
#undef N
#define P  (4U)
#define N  (P * CLOCK_CORECLOCK / PLL_IN_FREQ)
#if (N >= 50) && (N <= 432) && ((PLL_IN_FREQ * N / P) == CLOCK_CORECLOCK)
#define _PLL_N_FINISHED 1
#endif
#endif /* !_PLL_N_FINISHED */
#if !_PLL_N_FINISHED
#define P  (6U)
#define N  (P * CLOCK_CORECLOCK / PLL_IN_FREQ)
#if (N >= 50) && (N <= 432) && ((PLL_IN_FREQ * N / P) == CLOCK_CORECLOCK)
#define _PLL_N_FINISHED 1
#endif
#endif /* !_PLL_N_FINISHED */
#if !_PLL_N_FINISHED
#define P  (8U)
#define N  (P * CLOCK_CORECLOCK / PLL_IN_FREQ)
#if (N >= 50) && (N <= 432) && ((PLL_IN_FREQ * N / P) == CLOCK_CORECLOCK)
#define _PLL_N_FINISHED 1
#endif
#endif /* !_PLL_N_FINISHED */
#if !_PLL_N_FINISHED
#error "PLL configuration: no valid N/P pair found for PLL configuration"
#endif /* !_PLL_N_FINISHED */
#undef _PLL_N_FINISHED
#endif /* P */

/* finally we need to set Q, so that the USB clock is 48MHz */
#define Q                        ((P * CLOCK_CORECLOCK) / 48000000U)
#if ((Q * 48000000U) != (P * CLOCK_CORECLOCK))
#if defined(CPU_FAM_STM32F4) && !(defined(CPU_MODEL_STM32F446RE) || defined(CPU_MODEL_STM32F446ZE)) \
    && defined(RCC_DCKCFGR2_CK48MSEL)   /* cpu support 48MHz clock from PLL I2S */

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
#define CLOCK_48MHZ_2ND_PLL      (1)
#endif /* ((CLOCK_ENABLE_PLLI2S) && (CLOCK_I2S != 48000000U)) */

#elif defined(RCC_DCKCFGR2_CK48MSEL) /* cpu support 48Mhz clock from PLL SAI */
#if ((CLOCK_ENABLE_PLLSAI) && (CLOCK_SAI != 48000000U))
#error "PLL configuration: PLL SAI should be used for USB, but frequency is not 48MHz"
#else
/* Use PLL SAI @ 48MHz */
#ifndef CLOCK_ENABLE_PLLSAI
#define CLOCK_ENABLE_PLLSAI      (1)
#endif
#ifndef CLOCK_SAI
#define CLOCK_SAI                (48000000U)
#endif
#endif
#define CLOCK_48MHZ_2ND_PLL      (1)
#else
#error "PLL configuration: USB frequency is not 48MHz"
#endif
#endif /* ((Q * 48000000U) != (P * CLOCK_CORECLOCK)) */

#ifdef __cplusplus
}
#endif

#endif /* PLL_H */
/** @} */
