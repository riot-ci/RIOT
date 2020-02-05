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

uint_fast8_t cc26x2_cc13x2_rf_power_on(void)
{
    bool ints_disabled;
    bool switch_osc = false;

    /* Request the HF XOSC as the source for the HF clock. Needed before we can
     * use the FS. This will only request, it will _not_ perform the switch.
     */
    if (OSCClockSourceGet(OSC_SRC_CLK_HF) != OSC_XOSC_HF)
    {
        /* Request to switch to the crystal to enable radio operation. It takes a
         * while for the XTAL to be ready so instead of performing the actual
         * switch, we do other stuff while the XOSC is getting ready.
         */
        OSCClockSourceSet(OSC_SRC_CLK_HF, OSC_XOSC_HF);
        switch_osc = true;
    }

    /*
     * Trigger a switch to the XOSC, so that we can subsequently use the RF FS
     * This will block until the XOSC is actually ready, but give how we
     * requested it early on, this won't be too long a wait.
     * This should be done before starting the RAT.
     */
    if (switch_osc)
    {
        /* Block until the high frequency clock source is ready */
        while (!OSCHfSourceReady())
            ;

        /* Switch the HF clock source (cc26x2ware executes this from ROM) */
        OSCHfSourceSwitch();
    }

    ints_disabled = IntMasterDisable();

    /* Enable RF Core power domain */
    PRCMPowerDomainOn(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_ON) {}

    PRCMDomainEnable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet()) {}

    cc26x2_cc13x2_rf_setup_int();

    if (!ints_disabled)
    {
        IntMasterEnable();
    }

    /* Let CPE boot */
    RFCClockEnable();

    /* Send ping (to verify RF Core is ready and alive) */
    return cc26x2_cc13x2_rf_prop_execute_ping_cmd();
}

void cc26x2_cc13x2_rf_power_off(void)
{
    cc26x2_cc13x2_rf_stop_int();

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

uint_fast8_t cc26x2_cc13x2_rf_execute_abort_cmd(void)
{
    return (RFCDoorbellSendTo(CMDR_DIR_CMD(CMD_ABORT)) & 0xFF);
}

uint_fast8_t cc26x2_cc13x2_rf_prop_execute_ping_cmd(void)
{
    return (RFCDoorbellSendTo(CMDR_DIR_CMD(CMD_PING)) & 0xFF);
}

void cc26x2_cc13x2_rf_setup_int(void)
{
    bool ints_disabled;

    assert(PRCMRfReady());

    ints_disabled = IntMasterDisable();

    /* Set all interrupt channels to CPE0 channel, error to CPE1 */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEISL) = IRQ_INTERNAL_ERROR;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = IRQ_LAST_COMMAND_DONE | IRQ_LAST_FG_COMMAND_DONE;

    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntEnable(INT_RFC_CPE_0);
    IntEnable(INT_RFC_CPE_1);

    if (!ints_disabled)
    {
        IntMasterEnable();
    }
}

void cc26x2_cc13x2_rf_stop_int(void)
{
    bool ints_disabled;

    ints_disabled = IntMasterDisable();

    /* clear and disable interrupts */
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) = 0x0;
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = 0x0;

    IntUnregister(INT_RFC_CPE_0);
    IntUnregister(INT_RFC_CPE_1);
    IntPendClear(INT_RFC_CPE_0);
    IntPendClear(INT_RFC_CPE_1);
    IntDisable(INT_RFC_CPE_0);
    IntDisable(INT_RFC_CPE_1);

    if (!ints_disabled)
    {
        IntMasterEnable();
    }
}

void cc26x2_cc13x2_enable_vims(void)
{
    SetupTrimDevice();

    VIMSModeSet(VIMS_BASE, VIMS_MODE_ENABLED);
    VIMSConfigure(VIMS_BASE, true, true);
}
