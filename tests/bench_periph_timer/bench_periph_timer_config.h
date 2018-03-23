/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Configuration definitions for bench_periph_timer
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */
#ifndef BENCH_PERIPH_TIMER_CONFIG_H
#define BENCH_PERIPH_TIMER_CONFIG_H

#include <stdint.h>

#include "periph/timer.h"
#include "cpu.h"

#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

/**
 * @brief Timer under test (TUT)
 */
#ifndef TIM_TEST_DEV
#define TIM_TEST_DEV TIMER_DEV(1)
#endif
#ifndef TIM_TEST_FREQ
#define TIM_TEST_FREQ 1000000ul
#endif
#ifndef TIM_TEST_CHAN
#define TIM_TEST_CHAN 0
#endif

/* Use separate reference timer to compare against */
#ifndef USE_REFERENCE
#define USE_REFERENCE 1
#endif

/* Whether to keep statistics per timer target value, or only totals */
#ifndef DETAILED_STATS
#define DETAILED_STATS 1
#endif

/* Group statistics into log2 size buckets, instead of one record per timer target
 * i.e. 1, 2, 3-4, 5-8, 9-16, 17-32 etc. */
/* Only used if DETAILED_STATS is 1 */
#ifndef LOG2_STATS
#define LOG2_STATS 1
#endif

/* Margin to ensure that the rescheduling timer never is hit */
#ifndef RESCHEDULE_MARGIN
#define RESCHEDULE_MARGIN (SPIN_MAX_TARGET * 16)
#endif

/**
 * @brief Reference timer to compare against
 */
#ifndef TIM_REF_DEV
#define TIM_REF_DEV TIMER_DEV(0)
#endif
#ifndef TIM_REF_FREQ
#define TIM_REF_FREQ 1000000ul
#endif

#if !USE_REFERENCE
#undef TIM_REF_DEV
#undef TIM_REF_FREQ
#undef TIM_TEST_TO_REF
#define TIM_REF_DEV TIM_TEST_DEV
#define TIM_REF_FREQ TIM_TEST_FREQ
#define TIM_TEST_TO_REF(x) (x)
#endif

/* Longest timer timeout tested (TUT ticks)*/
/* Reduce this if RAM usage is too high */
#ifndef TEST_MAX
#define TEST_MAX 128
#endif
/* Shortest timer timeout tested (TUT ticks) */
#ifndef TEST_MIN
#if TIM_TEST_FREQ < 100000
/* this usually works for slow timers */
#define TEST_MIN 1
#else
/* avoid problems with timer_set_absolute setting a time in the past because of
 * processing delays */
#define TEST_MIN 16
#endif
#endif
/* Minimum delay for relative timers, should usually work with any value */
#ifndef TEST_MIN_REL
#define TEST_MIN_REL (0)
#endif
/* Number of test values */
#define TEST_NUM ((TEST_MAX) - (TEST_MIN) + 1)
/* 2-logarithm of TEST_NUM, not possible to compute automatically by the
 * preprocessor unless comparing values like this */
#if TEST_NUM <= 4
#define TEST_LOG2NUM 2
#elif TEST_NUM <= 8
#define TEST_LOG2NUM 3
#elif TEST_NUM <= 16
#define TEST_LOG2NUM 4
#elif TEST_NUM <= 32
#define TEST_LOG2NUM 5
#elif TEST_NUM <= 64
#define TEST_LOG2NUM 6
#elif TEST_NUM <= 128
#define TEST_LOG2NUM 7
#elif TEST_NUM <= 256
#define TEST_LOG2NUM 8
#elif TEST_NUM <= 512
#define TEST_LOG2NUM 9
#elif TEST_NUM <= 1024
#define TEST_LOG2NUM 10
#elif TEST_NUM <= 2048
#define TEST_LOG2NUM 11
#elif TEST_NUM <= 4096
#define TEST_LOG2NUM 12
#elif TEST_NUM <= 16384
#define TEST_LOG2NUM 14
#elif TEST_NUM <= 65536
#define TEST_LOG2NUM 16
#elif TEST_NUM <= 1048576
#define TEST_LOG2NUM 20
#elif TEST_NUM <= 16777216
#define TEST_LOG2NUM 24
#else
#define TEST_LOG2NUM 32
#endif

/* convert TUT ticks to reference ticks */
/* x is expected to be < 2**16 */
#ifndef TIM_TEST_TO_REF
#if (TIM_TEST_FREQ == TIM_REF_FREQ)
#define TIM_TEST_TO_REF(x) (x)
#elif (TIM_TEST_FREQ == 32768ul) && (TIM_REF_FREQ == 1000000ul)
#define TIM_TEST_TO_REF(x) (((uint32_t)(x) * 15625ul) >> 9)
#elif (TIM_TEST_FREQ == 1000000ul) && (TIM_REF_FREQ == 32768ul)
#define TIM_TEST_TO_REF(x) (div_u32_by_15625div512(x))
#endif
#endif

/* Print results every X reference ticks */
#ifndef TEST_PRINT_INTERVAL_TICKS
#define TEST_PRINT_INTERVAL_TICKS ((TIM_REF_FREQ) * 30)
#endif

/* If variance or mean exceeds these values the row will be marked with a "SIC!"
 * in the table output */
#ifndef TEST_UNEXPECTED_STDDEV
#define TEST_UNEXPECTED_STDDEV 4
#endif
#ifndef TEST_UNEXPECTED_MEAN
#define TEST_UNEXPECTED_MEAN 10
#endif

/* The spin calibration will try to set spin_limit to a number of loop
 * iterations which correspond to this many TUT ticks */
#ifndef SPIN_MAX_TARGET
#define SPIN_MAX_TARGET 16
#endif

/* estimate_cpu_overhead will loop for this many iterations to get a proper estimate */
#define ESTIMATE_CPU_ITERATIONS 2048

/* Results will be grouped by function, rescheduling yes/no, start/stop.
 * functions: timer_set, timer_set_absolute
 * reschedule: yes/no, when yes: first set one target time, before that time has
 * passed, set the real target time
 * start/stop: if stop: call timer_stop before setting the target time, then call timer_start
 *
 * All different variations will be mixed to provide the most varied input
 * vector possible for the benchmark. A more varied input should yield a more
 * correct estimate of the mean error and variance. Random CPU processing delays
 * will be inserted between each step to avoid phase locking the benchmark to
 * unobservable timer internals.
 */

enum {
    TEST_RESCHEDULE         = 1,
    TEST_STOPPED            = 2,
    TEST_ABSOLUTE           = 4,
    TEST_VARIANT_NUMOF      = 8,
};

#endif /* BENCH_PERIPH_TIMER_CONFIG_H */
