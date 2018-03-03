/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include "matstat.h"

void matstat_clear(matstat_state_t *state)
{
    *state = (const matstat_state_t){
        .min = INT32_MAX,
        .max = INT32_MIN,
        .count = 0,
        .sum = 0,
        .sum_sq = 0,
    };
}

void matstat_add(matstat_state_t *state, int32_t value)
{
    ++state->count;
    state->sum += value;
    /* Assuming the differences will be small on average and the number of
     * samples is reasonably limited, to prevent overflow in sum_sq */
    state->sum_sq += (int64_t)value * value;
    if (value > state->max) {
        state->max = value;
    }
    if (value < state->min) {
        state->min = value;
    }
}

int32_t matstat_mean(const matstat_state_t *state)
{
    int32_t mean = state->sum / state->count;
    return mean;
}

uint64_t matstat_variance(const matstat_state_t *state)
{
    if (state->count < 2) {
        return 0;
    }
    int32_t mean = matstat_mean(state);
    uint64_t variance = (state->sum_sq - state->count * mean * mean) / (state->count - 1);
    return variance;
}

void matstat_merge(matstat_state_t *dest, const matstat_state_t *src)
{
    dest->count += src->count;
    dest->sum += src->sum;
    dest->sum_sq += src->sum_sq;
    if (src->max > dest->max) {
        dest->max = src->max;
    }
    if (src->min < dest->min) {
        dest->min = src->min;
    }
}
