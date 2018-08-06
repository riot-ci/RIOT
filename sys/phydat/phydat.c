/*
 * Copyright (C) 2018 Eistec AB
 *               2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_phydat
 * @{
 *
 * @file
 * @brief       Generic sensor/actuator data handling
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdint.h>
#include "phydat.h"

#define ENABLE_DEBUG 0
#include "debug.h"

void phydat_fit(phydat_t *dat, const int32_t *values, unsigned int dim)
{
    assert(dim <= (sizeof(dat->val) / sizeof(dat->val[0])));
    long divisor = 1;
    long max = 0;

    /* Get the value with the highest magnitude */
    for (unsigned int i = 0; i < dim; i++) {
        if (values[i] > max) {
            max = values[i];
        }
        else if (-values[i] > max) {
            max = -values[i];
        }
    }

    /* Get the correct scale
     * (Not using max /= 10 here to prevent precision loss when rounding down)
     * (Not stopping at PHYDAT_MAX because rounding up could overflow)
     */
    while ((max / divisor) >= PHYDAT_MAX) {
        divisor *= 10;
        dat->scale++;
    }

    /* Applying scale and add half of the divisor for correct rounding */
    long divisor_half = divisor >> 1;
    for (unsigned int i = 0; i < dim; i++) {
        if (values[i] >= 0) {
            dat->val[i] = (values[i] + divisor_half) / divisor;
        }
        else {
            /* For negative integers the C standards seems to lack information
             * on whether to round down or towards zero. So using positive
             * integer division as last resort here.
             */
            dat->val[i] = -(((-values[i]) + divisor_half) / divisor);
        }
    }
}
