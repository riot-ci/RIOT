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

#include "bitarithm.h"
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
#define TEST_MIN 2
#endif
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
#ifndef TEST_UNEXPECTED_VARIANCE
#define TEST_UNEXPECTED_VARIANCE 10
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
#define ESTIMATE_CPU_ITERATIONS 256

/* variance due to errors in tick conversion */
static uint32_t conversion_variance = 0;

/* Limits for the mean and variance, to compare the results against expectation */
static int32_t expected_mean_low;
static int32_t expected_mean_high;
static uint32_t expected_variance_low = 0;
static uint32_t expected_variance_high = TEST_UNEXPECTED_VARIANCE;

/* Benchmark processing overhead, results will be compensated for this to make
 * the results easier to understand */
static int32_t overhead_target;
static int32_t overhead_read;

/* Seed for initializing the random module */
static uint32_t seed = 123;

/* Mutex used for signalling between main thread and ISR callback */
static mutex_t mtx_cb = MUTEX_INIT_LOCKED;

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

/* Test state element */
typedef struct {
    matstat_state_t *target_state; /* timer_set error statistics state */
    matstat_state_t *read_state; /* timer_read error statistics state */
    unsigned int target_ref; /* Target time in reference timer */
    unsigned int start_tut; /* Start time in timer under test */
    unsigned int start_ref; /* Start time in reference timer */
} test_ctx_t;

static test_ctx_t test_context;

#if DETAILED_STATS
#if LOG2_STATS
/* Group test values by 2-logarithm to reduce memory requirements */
static matstat_state_t states[TEST_VARIANT_NUMOF * TEST_LOG2NUM];
#else
/* State vector, first half will contain state for timer_set tests, second half
 * will contain state for timer_set_absolute */
static matstat_state_t states[TEST_VARIANT_NUMOF * TEST_NUM];
#endif
#else
/* Only keep stats per function variation */
static matstat_state_t states[TEST_VARIANT_NUMOF];
#endif

/* timer_read error statistics states */
static matstat_state_t read_states[TEST_VARIANT_NUMOF];

/* Callback for the timeout */
static void cb(void *arg, int chan)
{
    (void)chan;
    unsigned int now_ref = timer_read(TIM_REF_DEV);
    unsigned int now_tut = timer_read(TIM_TEST_DEV);
    if (arg == NULL) {
        print_str("cb: Warning! arg = NULL\n");
        return;
    }
    test_ctx_t *ctx = arg;
    if (ctx->target_state == NULL) {
        print_str("cb: Warning! target_state = NULL\n");
        return;
    }

    /* Update running stats */
    int32_t diff = now_ref - ctx->target_ref - overhead_target;
    matstat_add(ctx->target_state, diff);

    /* Update timer_read statistics only when timer_read has not overflowed
     * since the timer was set */
    if (now_tut >= ctx->start_tut) {
        diff = TIM_TEST_TO_REF(now_tut - ctx->start_tut) - overhead_read;
        diff -= (now_ref - ctx->start_ref);
        matstat_add(ctx->read_state, diff);
    }

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
    uint64_t variance = matstat_variance(state);

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
    if ((mean < expected_mean_low) || (expected_mean_high < mean) ||
        (variance < expected_variance_low) || (expected_variance_high < variance) ) {
        /* mean or variance is outside the expected range, alert the user */
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

static void print_detailed(const matstat_state_t *states, size_t nelem)
{
    if (LOG2_STATS) {
        print_str("   interval    count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int k = 0; k < nelem; ++k) {
            char buf[20];
            unsigned int num = (1 << k);
            if (num >= TEST_NUM) {
                break;
            }
            unsigned int start = num + TEST_MIN;
            if (num == 1) {
                /* special case, bitarithm_msb will return 0 for both 0 and 1 */
                start = TEST_MIN;
            }
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, start), 4, ' '));
            print_str(" - ");
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, TEST_MIN + (num * 2) - 1), 4, ' '));
            print_str(": ");
            print_statistics(&states[k]);
        }
        print_str("      TOTAL  ");
    }
    else {
        print_str("interval   count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int k = 0; k < nelem; ++k) {
            char buf[10];
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, k + TEST_MIN), 7, ' '));
            print_str(": ");
            print_statistics(&states[k]);
        }
        print_str("  TOTAL: ");
    }
    print_totals(states, nelem);
}

/**
 * @brief   Present the results of the benchmark
 *
 * Depends on DETAILED_STATS, LOG2_STATS
 */
static void print_results(void)
{
    print_str("------------- BEGIN STATISTICS --------------\n");
    print_str("Limits: mean: [");
    print_s32_dec(expected_mean_low);
    print_str(", ");
    print_s32_dec(expected_mean_high);
    print_str("], variance: [");
    print_u32_dec(expected_variance_low);
    print_str(", ");
    print_u32_dec(expected_variance_high);
    print_str("]\n");
    print_str("Target error (actual trigger time - expected trigger time)\n");
    print_str("positive: timer is late, negative: timer is early\n");

    if (DETAILED_STATS) {
        static unsigned int count = TEST_NUM;
        if (LOG2_STATS) {
            count = TEST_LOG2NUM;
        }
        print_str("=== timer_set running ===\n");
        print_detailed(&states[0], count);
        print_str("=== timer_set resched ===\n");
        print_detailed(&states[TEST_RESCHEDULE * count], count);
        print_str("=== timer_set stopped ===\n");
        print_detailed(&states[TEST_STOPPED * count], count);
        print_str("=== timer_set resched, stopped ===\n");
        print_detailed(&states[(TEST_RESCHEDULE | TEST_STOPPED) * count], count);
        print_str("=== timer_set_absolute running ===\n");
        print_detailed(&states[TEST_ABSOLUTE * count], count);
        print_str("=== timer_set_absolute resched ===\n");
        print_detailed(&states[(TEST_ABSOLUTE | TEST_RESCHEDULE) * count], count);
        print_str("=== timer_set_absolute stopped ===\n");
        print_detailed(&states[(TEST_ABSOLUTE | TEST_STOPPED) * count], count);
        print_str("=== timer_set_absolute resched, stopped ===\n");
        print_detailed(&states[(TEST_ABSOLUTE | TEST_RESCHEDULE | TEST_STOPPED) * count], count);
    }
    else {
        print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
        print_str(" timer_set          ");
        print_totals(&states[0], 4);
        print_str("  running           ");
        print_totals(&states[0], 1);
        print_str("  resched           ");
        print_totals(&states[TEST_RESCHEDULE], 1);
        print_str("  stopped           ");
        print_totals(&states[TEST_STOPPED], 1);
        print_str("  resched, stopped  ");
        print_totals(&states[TEST_RESCHEDULE | TEST_STOPPED], 1);
        print("\n", 1);
        print_str(" timer_set_absolute ");
        print_totals(&states[TEST_ABSOLUTE], 4);
        print_str("  running           ");
        print_totals(&states[TEST_ABSOLUTE], 1);
        print_str("  resched           ");
        print_totals(&states[TEST_ABSOLUTE | TEST_RESCHEDULE], 1);
        print_str("  stopped           ");
        print_totals(&states[TEST_ABSOLUTE | TEST_STOPPED], 1);
        print_str("  resched, stopped  ");
        print_totals(&states[TEST_ABSOLUTE | TEST_RESCHEDULE | TEST_STOPPED], 1);
    }
    print_str("=== timer_read statistics ===\n");
    print_str("timer_read error (TUT time elapsed - reference time elapsed)\n");
    print_str("positive: timer_read is speeding, negative: timer_read is dropping ticks\n");
    print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
    print_str(" timer_set          ");
    print_totals(&read_states[0], 4);
    print_str("  running           ");
    print_totals(&read_states[0], 1);
    print_str("  resched           ");
    print_totals(&read_states[TEST_RESCHEDULE], 1);
    print_str("  stopped           ");
    print_totals(&read_states[TEST_STOPPED], 1);
    print_str("  resched, stopped  ");
    print_totals(&read_states[TEST_RESCHEDULE | TEST_STOPPED], 1);
    print("\n", 1);
    print_str(" timer_set_absolute ");
    print_totals(&read_states[TEST_ABSOLUTE], 4);
    print_str("  running           ");
    print_totals(&read_states[TEST_ABSOLUTE], 1);
    print_str("  resched           ");
    print_totals(&read_states[TEST_ABSOLUTE | TEST_RESCHEDULE], 1);
    print_str("  stopped           ");
    print_totals(&read_states[TEST_ABSOLUTE | TEST_STOPPED], 1);
    print_str("  resched, stopped  ");
    print_totals(&read_states[TEST_ABSOLUTE | TEST_RESCHEDULE | TEST_STOPPED], 1);

    print_str("-------------- END STATISTICS ---------------\n");
}

/**
 * @brief   Select the proper state for the given test number depending on the
 *          compile time configuration
 *
 * Depends on DETAILED_STATS, LOG2_STATS
 */
static void assign_state_ptr(test_ctx_t *ctx, unsigned int num)
{
    unsigned int variant = num / TEST_NUM;
    ctx->read_state = &read_states[variant];
    if (DETAILED_STATS) {
        if (LOG2_STATS) {
            unsigned int log2num = bitarithm_msb(num % TEST_NUM);

            ctx->target_state = &states[variant * TEST_LOG2NUM + log2num];
        }
        else {
            ctx->target_state = &states[num];
        }
    }
    else {
        ctx->target_state = &states[variant];
    }
}

/**
 * @brief   Busy wait (spin) for the given number of loop iterations
 */
static void spin(uint32_t limit)
{
    /* Platform independent busy wait loop, should never be optimized out
     * because of the volatile asm statement */
    while (limit--) {
        __asm__ volatile ("");
    }
}

static uint32_t spin_max;

/**
 * @brief   Spin for a short random delay to increase fuzziness of the test
 */
static void spin_random_delay(void)
{
    uint32_t limit = random_uint32_range(0, spin_max);
    spin(limit);
}

static void calibrate_spin_max(void)
{
    print_str("Calibrating spin delay...\n");
    spin_max = 32;
    unsigned int t1;
    unsigned int t2;
    timer_start(TIM_TEST_DEV);
    do {
        spin_max *= 2;
        t1 = timer_read(TIM_TEST_DEV);
        spin(spin_max);
        t2 = timer_read(TIM_TEST_DEV);
    } while ((t2 - t1) < SPIN_MAX_TARGET);
    print_str("spin_max = ");
    print_u32_dec(spin_max);
    print("\n", 1);
}

static uint32_t run_test(test_ctx_t *ctx, unsigned int num)
{
    assign_state_ptr(ctx, num);
    uint32_t interval = (num % TEST_NUM) + TEST_MIN;
    spin_random_delay();
    unsigned int interval_ref = TIM_TEST_TO_REF(interval);
    unsigned int variant = num / TEST_NUM;
    if (variant & TEST_RESCHEDULE) {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval + RESCHEDULE_MARGIN);
        spin_random_delay();
    }
    if (variant & TEST_STOPPED) {
        timer_stop(TIM_TEST_DEV);
        spin_random_delay();
    }
    ctx->start_ref = timer_read(TIM_REF_DEV);
    ctx->start_tut = timer_read(TIM_TEST_DEV);
    ctx->target_ref = ctx->start_ref + interval_ref;
    if (variant & TEST_ABSOLUTE) {
        timer_set_absolute(TIM_TEST_DEV, TIM_TEST_CHAN, ctx->start_tut + interval);
    }
    else {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval);
    }
    if (variant & TEST_STOPPED) {
        spin_random_delay();
        /* do not update ctx->start_tut, because TUT should have been stopped
         * and not incremented during spin_random_delay */
        ctx->start_ref = timer_read(TIM_REF_DEV);
        ctx->target_ref = ctx->start_ref + interval_ref;
        timer_start(TIM_TEST_DEV);
    }
    mutex_lock(&mtx_cb);
    return interval;
}

static int test_timer(void)
{
    uint32_t time_begin = timer_read(TIM_REF_DEV);
    do {
        unsigned int num = (unsigned int)random_uint32_range(0, TEST_NUM * TEST_VARIANT_NUMOF);
        run_test(&test_context, num);
    } while((timer_read(TIM_REF_DEV) - time_begin) < TEST_PRINT_INTERVAL_TICKS);

    print_results();

    return 0;
}

static void estimate_cpu_overhead(void)
{
    /* Try to estimate the amount of CPU overhead between test start to test
     * finish to get a better reading */
    print_str("Estimating benchmark overhead...\n");
    uint32_t interval = 0;
    overhead_target = 0;
    overhead_read = 0;
    test_ctx_t context;
    test_ctx_t *ctx = &context;
    matstat_state_t target_state = MATSTAT_STATE_INIT;
    matstat_state_t read_state = MATSTAT_STATE_INIT;
    ctx->target_state = &target_state;
    ctx->read_state = &read_state;
    for (unsigned int k = 0; k < ESTIMATE_CPU_ITERATIONS; ++k) {
        unsigned int interval_ref = TIM_TEST_TO_REF(interval);
        ctx->start_ref = timer_read(TIM_REF_DEV);
        ctx->start_tut = timer_read(TIM_TEST_DEV);
        ctx->target_ref = ctx->start_ref + interval_ref;
        /* call yield to simulate a context switch to isr and back */
        thread_yield();
        cb(ctx, TIM_TEST_CHAN);
    }
    overhead_target = matstat_mean(&target_state);
    overhead_read = matstat_mean(&read_state);
    print_str("overhead_target = ");
    print_s32_dec(overhead_target);
    print_str("\n");
    print_str("overhead_read = ");
    print_s32_dec(overhead_read);
    print_str("\n");
}

int main(void)
{
    print_str("\nStatistical benchmark for timers\n");
    for (unsigned int k = 0; k < (sizeof(states) / sizeof(states[0])); ++k) {
        matstat_clear(&states[k]);
    }
    for (unsigned int k = 0; k < (sizeof(read_states) / sizeof(read_states[0])); ++k) {
        matstat_clear(&read_states[k]);
    }
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

    print_str("TEST_MIN = ");
    print_u32_dec(TEST_MIN);
    print("\n", 1);
    print_str("TEST_MAX = ");
    print_u32_dec(TEST_MAX);
    print("\n", 1);
    print_str("TEST_NUM = ");
    print_u32_dec(TEST_NUM);
    print("\n", 1);
    print_str("log2(TEST_NUM - 1) = ");
    unsigned log2test = bitarithm_msb(TEST_NUM - 1);
    print_u32_dec(log2test);
    print("\n", 1);
    print_str("state vector elements per variant = ");
    print_u32_dec(sizeof(states) / sizeof(states[0]) / TEST_VARIANT_NUMOF);
    print("\n", 1);
    print_str("number of variants = ");
    print_u32_dec(TEST_VARIANT_NUMOF);
    print("\n", 1);
    print_str("sizeof(state) = ");
    print_u32_dec(sizeof(states[0]));
    print_str(" bytes\n");
    print_str("state vector total memory usage = ");
    print_u32_dec(sizeof(states));
    print_str(" bytes\n");
    assert(log2test < TEST_LOG2NUM);
    print_str("TIM_TEST_DEV = ");
    print_u32_dec(TIM_TEST_DEV);
    print_str(", TIM_TEST_FREQ = ");
    print_u32_dec(TIM_TEST_FREQ);
    print_str(", TIM_TEST_CHAN = ");
    print_u32_dec(TIM_TEST_CHAN);
    print("\n", 1);
    print_str("TIM_REF_DEV  = ");
    print_u32_dec(TIM_REF_DEV);
    print_str(", TIM_REF_FREQ  = ");
    print_u32_dec(TIM_REF_FREQ);
    print("\n", 1);
    print_str("USE_REFERENCE = ");
    print_u32_dec(USE_REFERENCE);
    print("\n", 1);
    print_str("TEST_PRINT_INTERVAL_TICKS = ");
    print_u32_dec(TEST_PRINT_INTERVAL_TICKS);
    print("\n", 1);

    expected_mean_low = -TIM_TEST_TO_REF(TEST_UNEXPECTED_MEAN);
    expected_mean_high = TIM_TEST_TO_REF(TEST_UNEXPECTED_MEAN);
    if (TIM_TEST_FREQ < TIM_REF_FREQ) {
        /* The quantization errors should be uniformly distributed within +/- 0.5
         * test timer ticks of the reference time */
        /* The formula for the variance of a rectangle distribution on [a, b] is
         * Var = (b - a)^2 / 12 (taken directly from a statistics textbook)
         * Using (b - a)^2 == (b - a) * ((b + 1) - (a + 1)) gives a smaller
         * truncation error when using integer operations for converting the ticks */
        conversion_variance = ((TIM_TEST_TO_REF(1) - TIM_TEST_TO_REF(0)) *
            (TIM_TEST_TO_REF(2) - TIM_TEST_TO_REF(1))) / 12;
        /* The limits of the mean should account for the conversion error as well */
        int32_t mean_error = ((TIM_TEST_TO_REF(1) - TIM_TEST_TO_REF(0)) +
            (TIM_TEST_TO_REF(2) - TIM_TEST_TO_REF(1))) / 2;
        expected_mean_low -= mean_error;
        expected_mean_high += mean_error;
        expected_variance_low = conversion_variance - TEST_UNEXPECTED_VARIANCE;
        expected_variance_high = conversion_variance + TEST_UNEXPECTED_VARIANCE;
    }
    print_str("Expected error variance due to truncation in tick conversion: ");
    print_u32_dec(conversion_variance);
    print("\n", 1);
    int res = timer_init(TIM_REF_DEV, TIM_REF_FREQ, cb, NULL);
    if (res < 0) {
        print_str("Error ");
        print_s32_dec(res);
        print_str(" intializing reference timer\n");
        return res;
    }
    random_init(seed);

    res = timer_init(TIM_TEST_DEV, TIM_TEST_FREQ, cb, &test_context);
    if (res < 0) {
        print_str("Error ");
        print_s32_dec(res);
        print_str(" intializing timer under test\n");
        return res;
    }

    calibrate_spin_max();
    estimate_cpu_overhead();

    while(1) {
        test_timer();
    }

    return 0;
}
