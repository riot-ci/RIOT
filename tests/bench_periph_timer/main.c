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
 * @brief       Another peripheral timer test application
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "fmt.h"
#include "mutex.h"
#include "random.h"
#include "div.h"
#include "matstat.h"

#include "periph/timer.h"
#include "cpu.h"

#ifndef TIMER_NUMOF
#error "TIMER_NUMOF not defined!"
#endif

/**
 * @brief Timer under test (TUT)
 */
#ifndef TIM_TEST_DEV
#if TIMER_NUMOF > 1
#define TIM_TEST_DEV TIMER_DEV(1)
#else
#define TIM_TEST_DEV TIMER_DEV(0)
#endif
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
#define DETAILED_STATS 0
#endif

/* Perform more complex test where timers are rescheduled before firing */
#ifndef TEST_RESCHEDULE
#define TEST_RESCHEDULE 1
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
#define TEST_MIN 2
#endif
#endif
/* Number of test values */
#define TEST_NUM ((TEST_MAX) - (TEST_MIN) + 1)

/* convert TUT ticks to reference ticks */
/* x is expected to be < 2**16 */
#ifndef TIM_TEST_TO_REF
#if (TIM_TEST_FREQ == TIM_REF_FREQ)
#define TIM_TEST_TO_REF(x) (x)
#elif (TIM_TEST_FREQ == 32768ul) && (TIM_REF_FREQ == 1000000ul)
#define TIM_TEST_TO_REF(x) (((uint32_t)x * 15625ul) >> 9)
#elif (TIM_TEST_FREQ == 1000000ul) && (TIM_REF_FREQ == 32768ul)
#define TIM_TEST_TO_REF(x) (div_u32_by_15625div512(x))
#endif
#endif

/* Print results every X TUT ticks */
#ifndef TEST_PRINT_INTERVAL_TICKS
#define TEST_PRINT_INTERVAL_TICKS (TIM_TEST_FREQ * 15)
#endif

/* If variance or mean exceeds these values the row will be marked with a "SIC!"
 * in the table output */
#ifndef TEST_UNEXPECTED_VARIANCE
#define TEST_UNEXPECTED_VARIANCE 100
#endif
#ifndef TEST_UNEXPECTED_MEAN
#define TEST_UNEXPECTED_MEAN 100
#endif

/* Reference target */
static volatile unsigned int target = 0;

/* Seed for initializing the random module */
static uint32_t seed = 123;

/* Mutex used for signalling between main thread and ISR callback */
static mutex_t mtx_cb = MUTEX_INIT_LOCKED;

#if DETAILED_STATS
/* State vector, first half will contain state for timer_set tests, second half
 * will contain state for timer_set_absolute */
static matstat_state_t states[TEST_NUM * 2];
#else
/* Only keep stats per function */
static matstat_state_t states[2];
#endif

/* Callback for the timeout */
static void cb(void *arg, int chan)
{
    (void)chan;
    unsigned int now = timer_read(TIM_REF_DEV);
    int32_t diff = now - target;
    if (arg == NULL) {
        print_str("cb: Warning! arg = NULL\n");
        return;
    }
    matstat_state_t *state = *((matstat_state_t**)arg);
    if (state == NULL) {
        print_str("cb: Warning! state = NULL\n");
        return;
    }

    /* Update running stats */
    matstat_add(state, diff);

    mutex_unlock(&mtx_cb);
}

static void print_statistics(const matstat_state_t *state)
{
    if (state->count == 0) {
        print_str("no samples\n");
        return;
    }
    if (state->count == 1) {
        print_str("single sample: ");
        print_s32_dec((int32_t)state->sum);
        print("\n", 1);
        return;
    }
    int32_t mean = matstat_mean(state);
    uint64_t variance = matstat_variance(state, mean);

    char buf[20];
    print(buf, fmt_lpad(buf, fmt_u32_dec(buf, state->count), 7, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s64_dec(buf, state->sum), 9, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_u64_dec(buf, state->sum_sq), 12, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s32_dec(buf, state->min), 6, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s32_dec(buf, state->max), 5, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_s32_dec(buf, mean), 5, ' '));
    print(" ", 1);
    print(buf, fmt_lpad(buf, fmt_u64_dec(buf, variance), 6, ' '));
    if ((mean > TEST_UNEXPECTED_MEAN) || (-mean > TEST_UNEXPECTED_MEAN) ||
        (variance > TEST_UNEXPECTED_VARIANCE)) {
        /* mean or variance is greater than expected, alert the user */
        print_str("  <=== SIC!");
    }
    print("\n", 1);
}

static void print_totals(const matstat_state_t *states, size_t nelem)
{
    matstat_state_t totals;
    matstat_clear(&totals);
    for (size_t k = 0; k < nelem; ++k) {
        matstat_merge(&totals, &states[k]);
    }
    print_statistics(&totals);
}

static matstat_state_t *state = NULL;

static int test_timer(void)
{
    /* print test overview */
    print_str("Running timer test with seed ");
    print_u32_dec(seed);
    print_str(" using ");
#if MODULE_PRNG_MERSENNE
    print_str("Mersenne Twister PRNG.\n");
#elif MODULE_PRNG_MINSTD
    print_str("Park & Miller Minimal Standard PRNG.\n");
#elif MODULE_PRNG_MUSL_LCG
    print_str("Musl C PRNG.\n");
#elif MODULE_PRNG_TINYMT32
    print_str("Tiny Mersenne Twister PRNG.\n");
#elif MODULE_PRNG_XORSHIFT
    print_str("XOR Shift PRNG.\n");
#else
    print_str("unknown PRNG.\n");
#endif

    uint32_t duration = 0;
    do {
        unsigned int num = (unsigned int)random_uint32_range(0, TEST_NUM * 2);
        if (DETAILED_STATS) {
            state = &states[num];
        }
        else {
            if (num < TEST_NUM) {
                state = &states[0];
            }
            else {
                state = &states[1];
            }
        }
        unsigned int interval = num + TEST_MIN;
        if (num >= TEST_NUM) {
            interval -= TEST_NUM;
        }
        unsigned int interval_ref = TIM_TEST_TO_REF(interval);
        unsigned int now_ref = timer_read(TIM_REF_DEV);
        target = now_ref + interval_ref;
        if (TEST_RESCHEDULE) {
            timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval + 100);
        }
        if (num < TEST_NUM) {
            timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval);
        }
        else {
            unsigned int now = timer_read(TIM_TEST_DEV);
            timer_set_absolute(TIM_TEST_DEV, TIM_TEST_CHAN, now + interval);
        }
        mutex_lock(&mtx_cb);
        duration += interval;
    } while(duration < TEST_PRINT_INTERVAL_TICKS);

    print_str("------------- BEGIN STATISTICS --------------\n");

    if (DETAILED_STATS) {
        print_str("=== timer_set ===\n");
        print_str("interval   count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int i = 0; i < TEST_NUM; ++i) {
            char buf[10];
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, i + TEST_MIN), 7, ' '));
            print_str(": ");
            print_statistics(&states[i]);
        }
        print_str("  TOTAL: ");
        print_totals(&states[0], TEST_NUM);

        print_str("=== timer_set_absolute ===\n");
        print_str("interval   count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int i = 0; i < TEST_NUM; ++i) {
            char buf[10];
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, i + TEST_MIN), 7, ' '));
            print_str(": ");
            print_statistics(&states[i + TEST_NUM]);
        }
        print_str("  TOTAL: ");
        print_totals(&states[TEST_NUM], TEST_NUM);
    }
    else {
        print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
        print_str(" timer_set          ");
        print_totals(&states[0], 1);
        print_str(" timer_set_absolute ");
        print_totals(&states[1], 1);
    }

    print_str("-------------- END STATISTICS ---------------\n");

    return 0;
}

int main(void)
{
    print_str("\nStatistics test for peripheral timers\n");
    for (unsigned int k = 0; k < (sizeof(states) / sizeof(states[0])); ++k) {
        matstat_clear(&states[k]);
    }
    int res = timer_init(TIM_REF_DEV, TIM_REF_FREQ, cb, NULL);
    if (res < 0) {
        print_str("Error ");
        print_s32_dec(res);
        print_str(" intializing reference timer\n");
        return res;
    }
    random_init(seed);

    res = timer_init(TIM_TEST_DEV, TIM_TEST_FREQ, cb, &state);
    if (res < 0) {
        print_str("Error ");
        print_s32_dec(res);
        print_str(" intializing timer under test\n");
        return res;
    }

    while(1) {
        test_timer();
    }

    return 0;
}
