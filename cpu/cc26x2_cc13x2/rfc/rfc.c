/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 RF Core common functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "cc26x2_cc13x2_rfc.h"

#include <assert.h>

#include <driverlib/interrupt.h>
#include <driverlib/osc.h>
#include <driverlib/prcm.h>
#include <driverlib/rfc.h>
#include <driverlib/rf_common_cmd.h>

#include <driverlib/setup.h>
#include <driverlib/vims.h>

void cc26x2_cc13x2_rf_power_on(void)
{
    bool ints_disabled;

    /* Trigger a switch to the XOSC, so that we can subsequently use the RF
     * Frequency Synthesizer This will block until the XOSC is actually ready.
     */
    if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_XOSC_HF) {
        /* Request to switch to the crystal to enable radio operation. It takes a
         * while for the XTAL to be ready so instead of performing the actual
         * switch, we do other stuff while the XOSC is getting ready.
         */
        OSCClockSourceSet(OSC_SRC_CLK_HF, OSC_XOSC_HF);

        /* Block until the high frequency clock source is ready */
        while (!OSCHfSourceReady()) {}

        /* Switch the HF clock source (this get executed from ROM) */
        OSCHfSourceSwitch();
    }

    ints_disabled = IntMasterDisable();

    /* Enable RF Core power domain */
    PRCMPowerDomainOn(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_ON) {}

    PRCMDomainEnable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet()) {}

    if (!ints_disabled) {
        IntMasterEnable();
    }

    /* Let CPE boot */
    RFCClockEnable();
}

void cc26x2_cc13x2_rf_power_off(void)
{
    PRCMDomainDisable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet()) {}

    PRCMPowerDomainOff(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_OFF) {}

    if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_RCOSC_HF)
    {
        /* Request to switch to the RC osc for low power mode. */
        OSCClockSourceSet(OSC_SRC_CLK_HF, OSC_RCOSC_HF);
        /* Switch the HF clock source (cc26x2ware executes this from ROM) */
        OSCHfSourceSwitch();
    }
}

void cc26x2_cc13x2_enable_vims(void)
{
    SetupTrimDevice();

    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);
    VIMSConfigure(VIMS_BASE, true, true);
}
