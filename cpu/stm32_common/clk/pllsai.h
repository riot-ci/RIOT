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
 * @brief       STM32 PLL SAI configuration
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifndef PLLSAI_H
#define PLLSAI_H

#if (CLOCK_ENABLE_PLLSAI)
#ifndef RCC_CR_PLLSAIRDY
#error "No PLL SAI on this device"
#endif

#define PLLSAI_IN               PLL_IN

#if Q_SAI
#define N_SAI                   (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if ((N_SAI < 50) || (N_SAI > 432))
#error "PLL configuration: PLL SAI N value is out of range"
#endif
#if (PLL_IN_FREQ * N_SAI / Q_SAI) != CLOCK_SAI
#error "PLL configuration: PLL SAI N value is invalid (try with another Q_SAI)"
#endif
#else

#define Q_SAI  (2U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (3U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (4U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (5U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (6U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (7U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (8U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (9U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (10U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (11U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (12U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (13U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (14U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#endif /* _PLL_SAI_FINISHED */
#if !_PLL_SAI_FINISHED
#undef Q_SAI
#undef N_SAI
#define Q_SAI  (15U)
#define N_SAI  (Q_SAI * CLOCK_SAI / PLL_IN_FREQ)
#if (N_SAI >= 50) && (N_SAI <= 432) && ((PLL_IN_FREQ * N_SAI / Q_SAI) == CLOCK_SAI)
#define _PLL_SAI_FINISHED 1
#endif
#if !_PLL_SAI_FINISHED
#error "PLL configuration: no valid N/Q pair found for PLL SAI configuration"
#endif
#endif /* _PLL_SAI_FINISHED */
#undef _PLL_SAI_FINISHED
#endif /* Q_SAI */
#endif /* CLOCK_ENABLE_PLLSAI */

#endif /* PLLSAI_H */
/** @} */
