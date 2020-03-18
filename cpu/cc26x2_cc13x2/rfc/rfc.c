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

#define ENABLE_DEBUG (0)
#include "debug.h"

void rfc_power_on(void)
{
    DEBUG("rfc_power_on()\n");

    /* Trigger a switch to the XOSC, so that we can subsequently use the RF
     * Frequency Synthesizer This will block until the XOSC is actually ready.
     */
    if (osc_clock_source_get(OSC_SRC_CLK_HF) != OSC_XOSC_HF) {
        DEBUG("rfc_power_on: switching SCLK_HF to OSC_XOSC_HF\n");
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
    PRCM->PDCTL0RFC = 1;
    PRCM->PDCTL1RFC = 1;
    while (PRCM->PDSTAT1RFC != 1 && PRCM->PDSTAT0RFC != 1) {}

    /* Enable RF Core clocks */
    PRCM->RFCCLKG |= RFCCLKG_CLK_EN;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) ;

    irq_restore(key);

    /* Let CPE boot */
    rfc_clock_enable();
}

void rfc_power_off(void)
{
    DEBUG("rfc_power_off()\n");

    unsigned key = irq_disable();

    /* Disable RF Core clocks */
    PRCM->RFCCLKG &= ~RFCCLKG_CLK_EN;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) ;

    /* Power off RF Core domain */
    PRCM->PDCTL0RFC = 0;
    PRCM->PDCTL1RFC = 0;
    while (PRCM->PDSTAT1RFC == 1 && PRCM->PDSTAT0RFC == 1) {}

    if (osc_clock_source_get(OSC_SRC_CLK_HF) != OSC_RCOSC_HF) {
        DEBUG("rfc_power_off: switching SCLK_HF to OSC_RCOSC_HF\n");
        /* Request to switch to the RC osc for low power mode. */
        osc_clock_source_set(OSC_SRC_CLK_HF, OSC_RCOSC_HF);
        /* Switch the HF clock source (cc26x2ware executes this from ROM) */
        osc_hf_source_switch();
    }

    irq_restore(key);
}
