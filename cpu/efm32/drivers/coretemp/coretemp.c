/*
 * Copyright (C) 2018-2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32_drivers_coretemp
 * @{
 *
 * @file
 * @brief       Implementation of EFM32 internal temperature sensor
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <errno.h>

#include "board.h"
#include "coretemp.h"

#include "periph/adc.h"

#include "em_device.h"

int16_t coretemp_read(void)
{
    /* initialize factory calibration values */
#if defined(_SILICON_LABS_32B_SERIES_0)
    float cal_temp = (float)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT);
    float cal_value = (float)((DEVINFO->ADC0CAL2 & _DEVINFO_ADC0CAL2_TEMP1V25_MASK) >> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);
#else
    float cal_temp = (float)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT);
    float cal_value = (float)((DEVINFO->ADC0CAL3 & _DEVINFO_ADC0CAL3_TEMPREAD1V25_MASK) >> _DEVINFO_ADC0CAL3_TEMPREAD1V25_SHIFT);
#endif

    /* convert temperature channel */
    int32_t value = adc_sample(CORETEMP_ADC, ADC_RES_12BIT);

    /* convert sample to degrees Celsius, using the scaling factors from
       the data sheets */
#if defined(_SILICON_LABS_32B_SERIES_0)
    float temperature = cal_temp - ((cal_value - value) / -6.291456);
#else
    float temperature = cal_temp - ((cal_value - value) / -6.012928);
#endif

    return (int16_t)(temperature * 100.0);
}

int coretemp_init(void)
{
    /* sanity check to ensure the internal temperature sensor is selected */
#if defined(_SILICON_LABS_32B_SERIES_0)
    assert(adc_channel_config[CORETEMP_ADC].input == adcSingleInputTemp);
#else
    assert(adc_channel_config[CORETEMP_ADC].input == adcPosSelTEMP);
#endif

    /* initialize ADC */
    if (adc_init(CORETEMP_ADC) != 0) {
        return -EIO;
    }

    return 0;
}
