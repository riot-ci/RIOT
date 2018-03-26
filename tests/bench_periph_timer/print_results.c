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

#include "print_results.h"
#include "matstat.h"
#include "fmt.h"
#include "bench_periph_timer_config.h"

/* Test limits */
typedef struct {
    int32_t expected_mean_low;
    int32_t expected_mean_high;
    uint32_t expected_variance_low;
    uint32_t expected_variance_high;
} test_limits_t;

/* Limits for the mean and variance, to compare the results against expectation */
static test_limits_t target_limits;
static test_limits_t read_limits;

static void print_statistics(const matstat_state_t *state, const test_limits_t *limits)
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
    if (limits) {
        if ((mean < limits->expected_mean_low) || (limits->expected_mean_high < mean) ||
            (variance < limits->expected_variance_low) || (limits->expected_variance_high < variance) ) {
            /* mean or variance is outside the expected range, alert the user */
            print_str("  <=== SIC!");
        }
    }
    print("\n", 1);
}

static void print_totals(const matstat_state_t *states, size_t nelem, const test_limits_t *limits)
{
    matstat_state_t totals;
    matstat_clear(&totals);
    for (size_t k = 0; k < nelem; ++k) {
        matstat_merge(&totals, &states[k]);
    }
    print_statistics(&totals, limits);
}

static void print_detailed(const matstat_state_t *states, size_t nelem, unsigned int test_min)
{
    if (LOG2_STATS) {
        print_str("   interval    count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int k = 0; k < nelem; ++k) {
            char buf[20];
            unsigned int num = (1 << k);
            if (num >= TEST_NUM) {
                break;
            }
            unsigned int start = num + test_min;
            if (num == 1) {
                /* special case, bitarithm_msb will return 0 for both 0 and 1 */
                start = test_min;
            }
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, start), 4, ' '));
            print_str(" - ");
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, test_min + (num * 2) - 1), 4, ' '));
            print_str(": ");
            print_statistics(&states[k], &target_limits);
        }
        print_str("      TOTAL  ");
    }
    else {
        print_str("interval   count       sum       sum_sq    min   max  mean  variance\n");
        for (unsigned int k = 0; k < nelem; ++k) {
            char buf[10];
            print(buf, fmt_lpad(buf, fmt_u32_dec(buf, k + test_min), 7, ' '));
            print_str(": ");
            print_statistics(&states[k], &target_limits);
        }
        print_str("  TOTAL: ");
    }
    print_totals(states, nelem, &target_limits);
}

void print_results(const matstat_state_t *states, const matstat_state_t *read_states)
{
    print_str("------------- BEGIN STATISTICS --------------\n");
    print_str("Limits: mean: [");
    print_s32_dec(target_limits.expected_mean_low);
    print_str(", ");
    print_s32_dec(target_limits.expected_mean_high);
    print_str("], variance: [");
    print_u32_dec(target_limits.expected_variance_low);
    print_str(", ");
    print_u32_dec(target_limits.expected_variance_high);
    print_str("]\n");
    print_str("Target error (actual trigger time - expected trigger time), in reference timer ticks\n");
    print_str("positive: timer is late, negative: timer is early\n");

    if (DETAILED_STATS) {
        static const unsigned int count = ((LOG2_STATS) ? (TEST_LOG2NUM) : (TEST_NUM));
        print_str("=== timer_set running ===\n");
        print_detailed(&states[0], count, TEST_MIN_REL);
        print_str("=== timer_set resched ===\n");
        print_detailed(&states[TEST_RESCHEDULE * count], count, TEST_MIN_REL);
        print_str("=== timer_set stopped ===\n");
        print_detailed(&states[TEST_STOPPED * count], count, TEST_MIN_REL);
        print_str("=== timer_set resched, stopped ===\n");
        print_detailed(&states[(TEST_RESCHEDULE | TEST_STOPPED) * count], count, TEST_MIN_REL);
        print_str("=== timer_set_absolute running ===\n");
        print_detailed(&states[TEST_ABSOLUTE * count], count, TEST_MIN);
        print_str("=== timer_set_absolute resched ===\n");
        print_detailed(&states[(TEST_ABSOLUTE | TEST_RESCHEDULE) * count], count, TEST_MIN);
        print_str("=== timer_set_absolute stopped ===\n");
        print_detailed(&states[(TEST_ABSOLUTE | TEST_STOPPED) * count], count, TEST_MIN);
        print_str("=== timer_set_absolute resched, stopped ===\n");
        print_detailed(&states[(TEST_ABSOLUTE | TEST_RESCHEDULE | TEST_STOPPED) * count], count, TEST_MIN);
    }
    else {
        print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
        print_str(" timer_set          ");
        print_totals(&states[0], 4, &target_limits);
        print_str("  running           ");
        print_totals(&states[0], 1, &target_limits);
        print_str("  resched           ");
        print_totals(&states[TEST_RESCHEDULE], 1, &target_limits);
        print_str("  stopped           ");
        print_totals(&states[TEST_STOPPED], 1, &target_limits);
        print_str("  resched, stopped  ");
        print_totals(&states[TEST_RESCHEDULE | TEST_STOPPED], 1, &target_limits);
        print("\n", 1);
        print_str(" timer_set_absolute ");
        print_totals(&states[TEST_ABSOLUTE], 4, &target_limits);
        print_str("  running           ");
        print_totals(&states[TEST_ABSOLUTE], 1, &target_limits);
        print_str("  resched           ");
        print_totals(&states[TEST_ABSOLUTE | TEST_RESCHEDULE], 1, &target_limits);
        print_str("  stopped           ");
        print_totals(&states[TEST_ABSOLUTE | TEST_STOPPED], 1, &target_limits);
        print_str("  resched, stopped  ");
        print_totals(&states[TEST_ABSOLUTE | TEST_RESCHEDULE | TEST_STOPPED], 1, &target_limits);
    }
    print_str("=== timer_read statistics ===\n");
    print_str("Limits: mean: [");
    print_s32_dec(read_limits.expected_mean_low);
    print_str(", ");
    print_s32_dec(read_limits.expected_mean_high);
    print_str("], variance: [");
    print_u32_dec(read_limits.expected_variance_low);
    print_str(", ");
    print_u32_dec(read_limits.expected_variance_high);
    print_str("]\n");
    print_str("timer_read error (TUT time elapsed - expected TUT interval), in timer under test ticks\n");
    print_str("positive: timer target handling is slow, negative: timer_read is dropping ticks\n");
    print_str("function              count       sum       sum_sq    min   max  mean  variance\n");
    print_str(" timer_set          ");
    print_totals(&read_states[0], 4, &read_limits);
    print_str("  running           ");
    print_totals(&read_states[0], 1, &read_limits);
    print_str("  resched           ");
    print_totals(&read_states[TEST_RESCHEDULE], 1, &read_limits);
    print_str("  stopped           ");
    print_totals(&read_states[TEST_STOPPED], 1, &read_limits);
    print_str("  resched, stopped  ");
    print_totals(&read_states[TEST_RESCHEDULE | TEST_STOPPED], 1, &read_limits);
    print("\n", 1);
    print_str(" timer_set_absolute ");
    print_totals(&read_states[TEST_ABSOLUTE], 4, &read_limits);
    print_str("  running           ");
    print_totals(&read_states[TEST_ABSOLUTE], 1, &read_limits);
    print_str("  resched           ");
    print_totals(&read_states[TEST_ABSOLUTE | TEST_RESCHEDULE], 1, &read_limits);
    print_str("  stopped           ");
    print_totals(&read_states[TEST_ABSOLUTE | TEST_STOPPED], 1, &read_limits);
    print_str("  resched, stopped  ");
    print_totals(&read_states[TEST_ABSOLUTE | TEST_RESCHEDULE | TEST_STOPPED], 1, &read_limits);

    print_str("-------------- END STATISTICS ---------------\n");
}

void set_limits(void)
{
    target_limits.expected_mean_low = -(TEST_UNEXPECTED_MEAN);
    target_limits.expected_mean_high = (TEST_UNEXPECTED_MEAN);
    target_limits.expected_variance_low = 0;
    target_limits.expected_variance_high = (TEST_UNEXPECTED_STDDEV) * (TEST_UNEXPECTED_STDDEV);

    read_limits.expected_mean_low = -(TEST_UNEXPECTED_MEAN);
    read_limits.expected_mean_high = TEST_UNEXPECTED_MEAN;
    read_limits.expected_variance_low = 0;
    read_limits.expected_variance_high = (TEST_UNEXPECTED_STDDEV) * (TEST_UNEXPECTED_STDDEV);

    /* The quantization errors should be uniformly distributed within +/- 0.5
     * test timer ticks of the reference time */
    /* The formula for the variance of a rectangle distribution on [a, b] is
     * Var = (b - a)^2 / 12 (taken directly from a statistics textbook)
     * Using (b - a)^2 / 12 == (10b - 10a) * ((10b + 1) - (10a + 1)) / 1200
     * gives a smaller truncation error when using integer operations for
     * converting the ticks */
    uint32_t conversion_variance = ((TIM_TEST_TO_REF(10) - TIM_TEST_TO_REF(0)) *
        (TIM_TEST_TO_REF(11) - TIM_TEST_TO_REF(1))) / 1200;
    if (TIM_REF_FREQ > TIM_TEST_FREQ) {
        target_limits.expected_variance_low = ((TIM_TEST_TO_REF(10) - TIM_TEST_TO_REF(0) - 10 * (TEST_UNEXPECTED_STDDEV)) *
            (TIM_TEST_TO_REF(11) - TIM_TEST_TO_REF(1) - 10 * (TEST_UNEXPECTED_STDDEV))) / 1200;
        target_limits.expected_variance_high = ((TIM_TEST_TO_REF(10) - TIM_TEST_TO_REF(0) + 10 * (TEST_UNEXPECTED_STDDEV)) *
            (TIM_TEST_TO_REF(11) - TIM_TEST_TO_REF(1) + 10 * (TEST_UNEXPECTED_STDDEV))) / 1200;
        /* The limits of the mean should account for the conversion error as well */
        /* rounded towards positive infinity */
        int32_t mean_error = (TIM_TEST_TO_REF(128) - TIM_TEST_TO_REF(0) + 127) / 128;
        target_limits.expected_mean_high += mean_error;
    }

    print_str("Expected error variance due to truncation in tick conversion: ");
    print_u32_dec(conversion_variance);
    print("\n", 1);
}
