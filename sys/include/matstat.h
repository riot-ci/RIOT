/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef MATSTAT_H
#define MATSTAT_H

#include <stdint.h>

typedef struct {
    int64_t sum;
    uint64_t sum_sq;
    uint32_t count;
    int32_t min;
    int32_t max;
} matstat_state_t;

/**
 * @brief   Reset state
 */
void matstat_clear(matstat_state_t *state);

/**
 * @brief   Add another data point to the state
 */
void matstat_add(matstat_state_t *state, int32_t value);

int32_t matstat_mean(const matstat_state_t *state);

uint64_t matstat_variance(const matstat_state_t *state);

/**
 * @brief   Combine two states
 *
 * Adds the sums and counts, take the max of the max and min of the min
 */
void matstat_merge(matstat_state_t *dest, const matstat_state_t *src);
#endif /* MATSTAT_H */
