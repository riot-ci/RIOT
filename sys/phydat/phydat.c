/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_sensif
 * @{
 *
 * @file
 * @brief       Generic sensor/actuator data handling
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdint.h>
#include "phydat.h"

#define ENABLE_DEBUG 0
#include "debug.h"

int8_t phydat_fit(phydat_t *dat, long value, unsigned int index, int8_t prescale)
{
    assert(index < (sizeof(dat->val) / sizeof(dat->val[0])));
    int8_t ret = prescale;
    while (prescale > 0) {
        value = (value + 5) / 10;
        --prescale;
    }
    while (prescale < 0) {
        value *= 10;
        ++prescale;
    }
    int8_t scale_offset = 0;
    while ((value > PHYDAT_MAX) || (value < PHYDAT_MIN)) {
        value = (value + 5) / 10;
        for (unsigned int k = 0; k < (sizeof(dat->val) / sizeof(dat->val[0])); ++k) {
            dat->val[k] = (dat->val[k] + 5) / 10;
        }
        ++scale_offset;
    }
    dat->val[index] = value;
    dat->scale += scale_offset;
    ret += scale_offset;
    return ret;
}
