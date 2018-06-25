/**
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
 * @ingroup sys_random
 * @{
 * @file
 *
 * @brief PRNG seeding
 *
 * @author Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdint.h>

#include "log.h"
#include "luid.h"
#include "periph/cpuid.h"
#include "random.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

void auto_init_random(void)
{
    uint32_t seed;
#ifdef MODULE_PERIPH_CPUID
    luid_get(&seed, 4);
#else
    LOG_WARNING("random: NO SEED AVAILABLE!\n");
    seed = RANDOM_SEED_DEFAULT;
#endif
    DEBUG("random: using seed value %u\n", (unsigned)seed);
    random_init(seed);
}

void random_bytes(uint8_t *target, size_t n)
{
    uint32_t random;
    uint8_t *random_pos = (uint8_t*)&random;
    unsigned _n = 0;

    while (n--) {
        if (! (_n++ & 0x3)) {
            random = random_uint32();
            random_pos = (uint8_t *) &random;
        }
        *target++ = *random_pos++;
    }
}

uint32_t random_uint32_range(uint32_t a, uint32_t b)
{
    uint32_t range = b - a;
    uint32_t highest_pow2 = (1 << bitarithm_msb(range));
    uint32_t random = random_uint32();

    /* check if range is a power of two */
    if (range && !(range & (range - 1))) {
        /* modulo when divisor is power of two */
        return (random & (highest_pow2 - 1)) + a;
    }
    else {
        /* leftshift / multiply with 2 for next power of two interval and
         * reuse old variable */
        highest_pow2 = (highest_pow2 << 1);

        /* check if result is in uint32_t range */
        if (highest_pow2) {
            /* reuse variable to calculate (power-of-two)-modulo in loop below */
            highest_pow2 = highest_pow2 -1;
        }
        else {
            /* disable modulo operation in loop below */
            highest_pow2 = UINT32_MAX;
        }
        /* get random numbers until value is smaller than range */
        while (random >= range) {
            random = (random_uint32() & highest_pow2);
        }
        return (random + a);
    }
}
