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
 * @brief       Result printing function declarations
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef PRINT_RESULTS_H
#define PRINT_RESULTS_H

#include "matstat.h"

/**
 * @brief   Present the results of the benchmark
 *
 * Depends on DETAILED_STATS, LOG2_STATS
 * @param[in]   states      State vector from test run
 * @param[in]   read_states State vector from test run
 */
void print_results(const matstat_state_t *states, const matstat_state_t *read_states);

/**
 * @brief   Calculate the limits for mean and variance for this test
 */
void set_limits(void);

#endif /* PRINT_RESULTS_H */
