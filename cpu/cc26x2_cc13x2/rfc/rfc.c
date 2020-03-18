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

#include "cpu.h"
#include "cc26x2_cc13x2_rfc.h"

#undef UART0_BASE
#undef UART1_BASE
#undef GPT0_BASE
#undef GPT1_BASE
#undef GPT2_BASE
#undef GPT3_BASE
#undef GPIO_BASE
#undef FLASH_BASE
#undef AON_IOC_BASE

#include <assert.h>

#include <driverlib/prcm.h>
#include <driverlib/rfc.h>

void rfc_power_on(void)
{
    /* Trigger a switch to the XOSC, so that we can subsequently use the RF
     * Frequency Synthesizer This will block until the XOSC is actually ready.
     */
    if (osc_clock_source_get(OSC_SRC_CLK_HF) != OSC_XOSC_HF) {
        /* Request to switch to the crystal to enable radio operation. It takes
         * a while for the XTAL to be ready. */
        osc_clock_source_set(OSC_SRC_CLK_HF, OSC_XOSC_HF);

        /* Block until the high frequency clock source is ready */
        while (!osc_hf_source_ready()) {}

        /* Switch the HF clock source (this get executed from ROM) */
        osc_hf_source_switch();
    }

    unsigned key = irq_disable();

    /* Enable RF Core power domain */
    PRCMPowerDomainOn(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_ON)
        {}

    PRCMDomainEnable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet()) {}

    irq_restore(key);

    /* Let CPE boot */
    RFCClockEnable();
}

void rfc_power_off(void)
{
    PRCMDomainDisable(PRCM_DOMAIN_RFCORE);
    PRCMLoadSet();

    while (!PRCMLoadGet()) {}

    PRCMPowerDomainOff(PRCM_DOMAIN_RFCORE);

    while (PRCMPowerDomainStatus(PRCM_DOMAIN_RFCORE) != PRCM_DOMAIN_POWER_OFF)
        {}

    if (osc_clock_source_get(OSC_SRC_CLK_HF) != OSC_RCOSC_HF) {
        /* Request to switch to the RC osc for low power mode. */
        osc_clock_source_set(OSC_SRC_CLK_HF, OSC_RCOSC_HF);
        /* Switch the HF clock source (cc26x2ware executes this from ROM) */
        osc_hf_source_switch();
    }
}
