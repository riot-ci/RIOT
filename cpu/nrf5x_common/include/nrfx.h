/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2020 Inria
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_nrf5x_common
 * @{
 *
 * @file
 * @brief           nrfx compatibility layer
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Koen Zandberg <koen@bergzand.net>
 */

#ifndef NRFX_H
#define NRFX_H

#include "cpu_conf.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Manual override to always disable the DC/DC converter for stage 1
 *
 * On CPU models with a single-stage voltage regulator this will disable the
 * DC/DC for that stage. On dual-stage voltage regulator models, this will
 * disable the DC/DC converter for stage 1.
 */
#ifndef NRFX_DISABLE_DCDC_REG1
#define NRFX_DISABLE_DCDC_REG1      0
#endif

/**
 * @brief   Manual override to always disable the DC/DC converter for stage 0
 *
 * This has only effect on CPU models that implement a two stage voltage
 * regulator (e.g. nrf52840).
 */
#ifndef NRFX_DISABLE_DCDC_REG0
#define NRFX_DISABLE_DCDC_REG0      0
#endif

/**
 * @brief Enable the internal DC/DC power converter for the NRF5x MCU.
 *
 * In most cases, the internal DC/DC converter is more efficient compared to the
 * LDO regulator. The downside of the DC/DC converter is that it requires an
 * external LC filter to be present on the board. Per default, the DC/DC
 * converter is enabled if an LC filter is present (VDD_LC_FILTER_REGx feature).
 *
 * Independent of the presence of the LC filter, the DC/DC stage can be disabled
 * by setting NRFX_DISABLE_DCDC_REGx to 1.
 */
static inline void nrfx_dcdc_init(void)
{
    if (IS_ACTIVE(MODULE_VDD_LC_FILTER_REG1) &&
        !IS_ACTIVE(NRFX_DISABLE_DCDC_REG1)) {
        NRF_POWER->DCDCEN = 1;
    }

#ifdef POWER_MAINREGSTATUS_MAINREGSTATUS_High
    /* on CPUs that support high voltage power supply via VDDH and thus use a
     * two stage regulator, we also try to enable the DC/DC converter for the
     * first stage */
    if (IS_ACTIVE(MODULE_VDD_LC_FILTER_REG0) &&
        !IS_ACTIVE(NRFX_DISABLE_DCDC_REG0) &&
        (NRF_POWER->MAINREGSTATUS == POWER_MAINREGSTATUS_MAINREGSTATUS_High)) {
        NRF_POWER->DCDCEN0 = 1;
    }
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* NRFX_H */
/** @} */
