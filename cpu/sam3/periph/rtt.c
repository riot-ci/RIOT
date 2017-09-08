/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_sam3
 * @ingroup     drivers_periph_rtt
 *
 * @note        The hardware RTT unit does neither support overflow interrupts
 *              nor setting the counter value. For this, this RTT driver does
 *              not implement those functions.
 * @{
 *
 * @file
 * @brief       Low-level RTT driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "periph/rtt.h"

#define ENABLE_DEBUG        (1)
#include "debug.h"

/* guard file to prevent file being build if no RTT was configured */
#if RTT_NUMOF

static struct {
    rtt_cb_t cb;
    void *arg;
} isr_ctx;

void rtt_init(void)
{
    /* enable RTT module */
    rtt_poweron();
    /* configure and apply the pre-scaler */
    uint16_t pre = (CLOCK_SCLK / RTT_FREQUENCY);
    RTT->RTT_MR = RTT_MR_RTPRES(pre);
    RTT->RTT_MR |= RTT_MR_RTTRST;
    DEBUG("[rtt] setting prescaler to %i\n", (int)pre);
    /* configure NVIC line */
    NVIC_EnableIRQ(RTT_IRQn);
}

uint32_t rtt_get_counter(void)
{
    return RTT->RTT_VR;
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* cancel any existing alarm */
    RTT->RTT_MR &= ~(RTT_MR_ALMIEN);
    /* set new alarm */
    isr_ctx.cb  = cb;
    isr_ctx.arg = arg;
    RTT->RTT_AR = alarm;
    DEBUG("[rtt] set new alarm to trigger at %u\n", (unsigned)alarm);
    /* (re-)enable the alarm */
    RTT->RTT_MR |= RTT_MR_ALMIEN;
}

uint32_t rtt_get_alarm(void)
{
    if (RTT->RTT_MR & RTT_MR_ALMIEN) {
        return RTT->RTT_AR;
    }
    return 0;
}

void rtt_clear_alarm(void)
{
    RTT->RTT_MR &= ~(RTT_MR_ALMIEN);
}

void rtt_poweron(void)
{
    PMC->PMC_PCER0 |= (1 << ID_RTT);
}

void rtt_poweroff(void)
{
    PMC->PMC_PCER0 &= ~(1 << ID_RTT);
}

void isr_rtt(void)
{
    uint32_t state = RTT->RTT_SR;       /* this clears all pending flags */
    DEBUG("[rtt] ISR: state is 0x%08x\n", (int)state);
    if (state & RTT_SR_ALMS) {
        RTT->RTT_MR &= ~(RTT_MR_ALMIEN);
        isr_ctx.cb(isr_ctx.arg);
    }

    cortexm_isr_end();
}

#endif
