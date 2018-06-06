/*
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the random number generator based of SRAM
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 *
 * @}
 */
#include <stdio.h>

extern uint32_t global_puf_seed;

int main(void)
{
    puts("Start: Test random number generator");

    printf("Success: Data for global_puf_seed: [0x%08lX]",global_puf_seed);

    puts("End: Test finished");
    return 0;
}
