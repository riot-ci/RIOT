/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_saul
 * @{
 *
 * @file
 * @brief       Auto initialization of internal voltage sensor directly mapped to SAUL reg
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include "cpu.h"
#include "log.h"
#include "saul_reg.h"
#include "saul/periph.h"
#include "periph/adc.h"

/* ADC uses ¼ Vdd for Vref amplified by 4 */
#define VDD_REF_mV  (3300)

static int _read_voltage(const void *dev, phydat_t *res)
{
    (void)dev;

    int raw = adc_sample(NRF52_VDDHDIV5, ADC_RES_12BIT) * 5 * VDD_REF_mV;
    res->val[0] = raw >> 12;
    res->unit = UNIT_V;
    res->scale = -3;

    return 1;
}

static saul_driver_t nrf_vddh_saul_driver = {
    .read = _read_voltage,
    .write = saul_notsup,
    .type = SAUL_SENSE_VOLTAGE,
};

void auto_init_nrf_vddh(void)
{
    static saul_reg_t saul_reg_entry = {
        .dev = NULL,
        .name = "NRF_VDDH",
        .driver = &nrf_vddh_saul_driver,
    };

    adc_init(NRF52_VDDHDIV5);

    /* add to registry */
    saul_reg_add(&(saul_reg_entry));
}
