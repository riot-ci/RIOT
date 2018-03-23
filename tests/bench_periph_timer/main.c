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
#include "spin_random.h"

#include "periph/timer.h"
#include "cpu.h"

#include "print_results.h"
#include "bench_periph_timer_config.h"

/* Benchmark processing overhead, results will be compensated for this to make
 * the results easier to understand */
static int32_t overhead_target;
static int32_t overhead_read;

/* Seed for initializing the random module */
static uint32_t seed = 123;

/* Mutex used for signalling between main thread and ISR callback */
static mutex_t mtx_cb = MUTEX_INIT_LOCKED;

/* Test state element */
typedef struct {
    matstat_state_t *target_state; /* timer_set error statistics state */
    matstat_state_t *read_state; /* timer_read error statistics state */
    unsigned int target_ref; /* Target time in reference timer */
    unsigned int target_tut; /* Target time in timer under test */
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
    unsigned int now_tut = timer_read(TIM_TEST_DEV);
    unsigned int now_ref = timer_read(TIM_REF_DEV);
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
    /* When setting a timer with a timeout of X ticks, we expect the
     * duration between the set and the callback, dT, to be at least
     * X * time_per_tick.
     * In order to ensure that dT <= X * time_per_tick, the timer read value
     * will actually have incremented (X + 1) times during that period,
     * because the set can occur asynchrously anywhere between timer
     * increments. Therefore, in this test, we consider (X + 1) to be the
     * expected timer_read value at the point the callback is called.
     */

    /* Check that reference timer did not overflow during the test */
    if ((now_ref + 0x4000u) >= ctx->target_ref) {
        int32_t diff = now_ref - ctx->target_ref - 1 - overhead_target;
        matstat_add(ctx->target_state, diff);
    }
    /* Update timer_read statistics only when timer_read has not overflowed
     * since the timer was set */
    if ((now_tut + 0x4000u) >= ctx->target_tut) {
        int32_t diff = now_tut - ctx->target_tut - 1 - overhead_read;
        matstat_add(ctx->read_state, diff);
    }

    mutex_unlock(&mtx_cb);
}

/**
 * @brief   Select the proper state for the given test number depending on the
 *          compile time configuration
 *
 * Depends on DETAILED_STATS, LOG2_STATS
 */
static void assign_state_ptr(test_ctx_t *ctx, unsigned int variant, uint32_t interval)
{
    ctx->read_state = &read_states[variant];
    if (DETAILED_STATS) {
        if (LOG2_STATS) {
            unsigned int log2num = bitarithm_msb(interval);

            ctx->target_state = &states[variant * TEST_LOG2NUM + log2num];
        }
        else {
            ctx->target_state = &states[variant * TEST_NUM + interval];
        }
    }
    else {
        ctx->target_state = &states[variant];
    }
}

static uint32_t derive_interval(uint32_t num)
{
    uint32_t interval;
    if ((DETAILED_STATS) && (LOG2_STATS)) {
        /* Use a logarithmic method to generate geometric variates in order to
         * populate the result table evenly across all buckets */

        /* Static exponent mask, picking the mask as tightly as possible reduces the
         * probability of discarded values, which reduces the computing overhead
         * between test iterations */

        static uint32_t exp_mask = 0;
        if (exp_mask == 0) {
            /* non-constant initializer */
            exp_mask = (2 << bitarithm_msb(TEST_LOG2NUM)) - 1;
            print_str("exp_mask = ");
            print_u32_hex(exp_mask);
            print("\n", 1);
            print_str("max interval = ");
            print_u32_dec((2 << exp_mask) - 1);
            print("\n", 1);
        }

        /* Pick an exponent based on the top bits of the number */
        /* exponent will be a number in the interval [0, log2(TEST_NUM) + 1] */
        unsigned int exponent = ((num >> (32 - 8)) & exp_mask);
        if (exponent == 0) {
            /* Special handling to avoid the situation where we never see a zero */
            /* We could also have used an extra right shift in the else case,
             * but the state grouping also groups 0 and 1 in the same bucket, which means that they are twice as likely  */
            interval = bitarithm_bits_set(num) & 1;
        }
        else {
            interval = (1 << exponent);
            interval |= (num & (interval - 1));
        }
    }
    else {
        static const uint32_t mask = (1 << TEST_LOG2NUM) - 1;
        interval = num & mask;
    }
    return interval;
}

static uint32_t run_test(test_ctx_t *ctx, uint32_t num)
{
    unsigned int variant = (num >> (32 - 3));
    if (variant >= TEST_VARIANT_NUMOF) {
        return 0;
    }
    uint32_t interval = derive_interval(num);
    if (interval >= TEST_NUM) {
        /* Discard values outside our test range */
        return 0;
    }
    assign_state_ptr(ctx, variant, interval);
    spin_random_delay();
    if (variant & TEST_ABSOLUTE) {
        interval += TEST_MIN;
    }
    else {
        interval += TEST_MIN_REL;
    }
    unsigned int interval_ref = TIM_TEST_TO_REF(interval);

    if (variant & TEST_RESCHEDULE) {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval + RESCHEDULE_MARGIN);
        spin_random_delay();
    }
    if (variant & TEST_STOPPED) {
        timer_stop(TIM_TEST_DEV);
        spin_random_delay();
    }
    ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
    ctx->target_tut = timer_read(TIM_TEST_DEV) + interval;
    if (variant & TEST_ABSOLUTE) {
        timer_set_absolute(TIM_TEST_DEV, TIM_TEST_CHAN, ctx->target_tut);
    }
    else {
        timer_set(TIM_TEST_DEV, TIM_TEST_CHAN, interval);
    }
    if (variant & TEST_STOPPED) {
        spin_random_delay();
        /* do not update ctx->target_tut, because TUT should have been stopped
         * and not incremented during spin_random_delay */
        ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref;
        timer_start(TIM_TEST_DEV);
    }
    mutex_lock(&mtx_cb);
    return interval;
}

static int test_timer(void)
{
    uint32_t time_last = timer_read(TIM_REF_DEV);
    uint32_t time_elapsed = 0;
    do {
        uint32_t num = random_uint32();

        run_test(&test_context, num);
        uint32_t now = timer_read(TIM_REF_DEV);
        if (now >= time_last) {
            /* Account for reference timer possibly overflowing before 30 seconds have passed */
            time_elapsed += now - time_last;
        }
        time_last = now;
    } while(time_elapsed < TEST_PRINT_INTERVAL_TICKS);

    print_results(&states[0], &read_states[0]);

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
        spin_random_delay();
        ctx->target_tut = timer_read(TIM_TEST_DEV) + interval - 1;
        ctx->target_ref = timer_read(TIM_REF_DEV) + interval_ref - 1;
        /* call yield to simulate a context switch to isr and back */
        thread_yield_higher();
        cb(ctx, TIM_TEST_CHAN);
    }
    overhead_target = matstat_mean(&target_state);
    overhead_read = matstat_mean(&read_state);
    print_str("overhead_target = ");
    print_s32_dec(overhead_target);
    print_str(" (s2 = ");
    uint32_t var = matstat_variance(&target_state);
    print_u32_dec(var);
    print_str(")\n");
    if (var > 2) {
        print_str("Warning: Variance in CPU estimation is too high\n");
#ifdef CPU_NATIVE
        print_str("This is expected on native when other processes are running\n");
#endif
    }
    print_str("overhead_read = ");
    print_s32_dec(overhead_read);
    print_str(" (s2 = ");
    var = matstat_variance(&read_state);
    print_u32_dec(var);
    print_str(")\n");
    if (var > 2) {
        print_str("Warning: Variance in CPU estimation is too high\n");
#ifdef CPU_NATIVE
        print_str("This is expected on native when other processes are running\n");
#endif
    }
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
    print_str("TEST_MIN_REL = ");
    print_u32_dec(TEST_MIN_REL);
    print("\n", 1);
    print_str("TEST_MAX_REL = ");
    print_u32_dec(TEST_MIN_REL + TEST_NUM - 1);
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

    if (TEST_MAX > 512) { /* Arbitrarily chosen limit */
        print_str("Warning: Using long intervals for testing makes the result "
                  "more likely to be affected by clock drift between the "
                  "reference timer and the timer under test. This can be "
                  "detected as a skewness in the mean values between different "
                  "intervals in the results table.\n");
        if (LOG2_STATS) {
            print_str("The variance of the larger intervals may also be greater "
                      "than expected if there is significant clock drift across "
                      "the bucketed time frame\n");
        }

    }
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

    set_limits();

    print_str("Calibrating spin delay...\n");
    uint32_t spin_max = spin_random_calibrate(TIM_TEST_DEV, SPIN_MAX_TARGET);
    print_str("spin_max = ");
    print_u32_dec(spin_max);
    print("\n", 1);
    estimate_cpu_overhead();

    while(1) {
        test_timer();
    }

    return 0;
}
