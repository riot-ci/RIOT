/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   boards_frdm-k64f
 * @{
 *
 * @file
 * @brief     Board specific configuration of direct mapped ADC
 *
 * @author    Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef ADC_PARAMS_H
#define ADC_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const  saul_adc_params_t saul_adc_params[] =
{
    { .name = "PTB2",     .line = ADC_LINE(0), .res  = ADC_RES_16BIT, },
    { .name = "PTB3",     .line = ADC_LINE(1), .res  = ADC_RES_16BIT, },
    { .name = "PTB10",    .line = ADC_LINE(2), .res  = ADC_RES_16BIT, },
    { .name = "PTB11",    .line = ADC_LINE(3), .res  = ADC_RES_16BIT, },
    { .name = "PTC11",    .line = ADC_LINE(4), .res  = ADC_RES_16BIT, },
    { .name = "PTC10",    .line = ADC_LINE(5), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP0", .line = ADC_LINE(6), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DM0", .line = ADC_LINE(7), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP0-ADC0_DM0", .line = ADC_LINE(8), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP0", .line = ADC_LINE(9), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DM0", .line = ADC_LINE(10), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP0-ADC1_DM0", .line = ADC_LINE(11), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP1", .line = ADC_LINE(12), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DM1", .line = ADC_LINE(13), .res  = ADC_RES_16BIT, },
    { .name = "ADC0_DP1-ADC0_DM1", .line = ADC_LINE(14), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP1", .line = ADC_LINE(15), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DM1", .line = ADC_LINE(16), .res  = ADC_RES_16BIT, },
    { .name = "ADC1_DP1-ADC1_DM1", .line = ADC_LINE(17), .res  = ADC_RES_16BIT, },
    {
        .name = "coretemp", .line = ADC_LINE(18), .res  = ADC_RES_16BIT,
        .unit = UNIT_TEMP_C, .scale = -1,
        /* Scaling the temperature sensor voltage to units of 0.1 Celsius, this
         * depends on several numbers found in the data sheet as well as the
         * analog reference voltage (VREFH), which is 3.3 V on FRDM-K64F.
         * These numbers yield a full range of -1570..466 Cel, which is
         * unrealistic, but because of the physical limits, the sensor will ever
         * only use a small range of that interval. */
        .val_min = (int32_t)(250 - ((   0 - 716) / 1.62) * 10),
        .val_max = (int32_t)(250 - ((3300 - 716) / 1.62) * 10),
    },
    { .name = "corebandgap", .line = ADC_LINE(19), .res  = ADC_RES_16BIT, },
};

#ifdef __cplusplus
}
#endif

#endif /* ADC_PARAMS_H */
/** @} */
