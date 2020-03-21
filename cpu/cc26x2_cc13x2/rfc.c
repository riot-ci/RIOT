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

#include <driverlib/rfc.h>
#include <inc/hw_aon_rtc.h>

void rfc_clock_enable(void)
{
    /* TODO: this is a temporary solution, this should be removed along with
     * driverlib in order to use RIOT definitions (which is a constant WIP).
     */
    RFCClockEnable();
}

uint8_t rfc_execute_sync(uint32_t cmd)
{
    return RFCDoorbellSendTo(cmd) & 0xFF;
}

uint32_t rfc_cpe_int_get(uint32_t flag)
{
    return HWREG(RFC_DBELL_BASE + RFC_DBELL_O_RFCPEIFG) & flag;
}

void rfc_cpe_int_clear(uint32_t flag)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIFG) &= ~flag;
}

void rfc_cpe_int_enable_clear(void)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) = 0x0;
}

void rfc_cpe_int_enable(uint32_t irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) |= irq;
}

void rfc_cpe_int_disable(uint32_t irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEIEN) &= ~irq;
}

void rfc_cpe0_int_select(uint32_t irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEISL) &= ~irq;
}

void rfc_cpe1_int_select(uint32_t irq)
{
    HWREG(RFC_DBELL_NONBUF_BASE + RFC_DBELL_O_RFCPEISL) |= irq;
}

void aon_rtc_ctl_rtc_upd(bool v)
{
    /* Turn on the clock line to the radio core, this is necessary to use the
     * CMD_SYNC_START_RAT and the CMD_SYNC_STOP_RAT commands. */
    HWREGBITW(AON_RTC_BASE + AON_RTC_O_CTL, AON_RTC_CTL_RTC_UPD_EN_BITN) = v ? 1 : 0;
}