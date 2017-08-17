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
 * @brief       STM32 PLL I2S configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifndef PLLI2S_H
#define PLLI2S_H

#ifdef __cplusplus
extern "C" {
#endif

#if (CLOCK_ENABLE_PLLI2S)
#ifndef RCC_CR_PLLI2SRDY
#error "No PLL I2S on this device"
#endif
/* Define CLOCK_I2S_CKIN to use external AFI clock, otherwise, use PLL_IN */
#if (CLOCK_I2S_CKIN) && defined(RCC_PLLI2SCFGR_PLLI2SSRC)
#define PLLI2S_IN               CLOCK_I2S_CKIN
#define PLLI2S_SRC              RCC_PLLI2SCFGR_PLLI2SSRC
#else
#define PLLI2S_IN               PLL_IN
#define PLLI2S_SRC              (0)
#endif

#ifdef RCC_PLLI2SCFGR_PLLI2SM
#define M_I2S                   (PLLI2S_IN / PLL_IN_FREQ)
#if ((M_I2S < 2) || (M_I2S > 63))
#error "PLL configuration: PLL I2S M value is out of range"
#endif
#if (PLL_IN_FREQ * M_I2S) != PLLI2S_IN
#error "PLL configuration: PLL I2S input frequency is invalid (M)"
#endif
#else
#define M_I2S                   (0)
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
/* N/Q pair condition */
#define Q_N_COND N_PQR_COND(N_I2S, Q_I2S, PLL_IN_FREQ, CLOCK_I2S)
/* Try for valid Q values (3,..,15) */
#define Q_I2S  (2U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (3U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (4U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (5U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (6U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (7U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (8U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (9U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (10U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (11U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (12U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (13U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (14U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#endif /* _PLL_I2S_FINISHED */
#if !_PLL_I2S_FINISHED
#undef Q_I2S
#undef N_I2S
#define Q_I2S  (15U)
#define N_I2S  (Q_I2S * CLOCK_I2S / PLL_IN_FREQ)
#if Q_N_COND
#define _PLL_I2S_FINISHED 1
#endif
#if !_PLL_I2S_FINISHED
#error "PLL configuration: no valid N/Q pair found for PLL I2S configuration"
#endif
#endif /* _PLL_I2S_FINISHED */
#undef _PLL_I2S_FINISHED
#undef Q_N_COND
#endif /* Q_I2S */
#endif /* CLOCK_ENABLE_PLLI2S */

#ifdef __cplusplus
}
#endif

#endif /* PLLI2S_H */
/** @} */
