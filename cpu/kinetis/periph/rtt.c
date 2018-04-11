/*
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_rtt
 *
 * @{
 *
 * @file
 * @brief       Low-level RTT interface implementation for Freescale Kinetis
 *              MCUs. NXP's RTC module is what RIOT calls a Real-Time
 *              Timer (RTT), a simple counter which counts seconds; RIOT Real-
 *              Time Clocks (RTC) counts seconds, minutes, hours etc. We provide
 *              an RTT->RTC wrapper layer in a separate file to allow using the
 *              RTT as a system real time clock.
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <time.h>
#include "cpu.h"
#include "bit.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef struct {
    rtt_cb_t alarm_cb;              /**< callback called from RTC alarm */
    void *alarm_arg;                /**< argument passed to the callback */
    rtt_cb_t overflow_cb;           /**< callback called when RTC overflows */
    void *overflow_arg;             /**< argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_callback;

void rtt_init(void)
{
    RTC_Type *rtt = RTT_DEV;

    /* Enable module clock gate */
    RTC_CLKEN();

    /* At this point, the CPU core may be clocked by a clock derived from the
     * RTC oscillator, avoid touching the oscillator enable bit (OSCE) in RTC_CR */

    /* Enable user mode access */
    bit_set32(&rtt->CR, RTC_CR_SUP_SHIFT);

    /* Disable all RTC interrupts. */
    rtt->IER = 0;

    /* The RTC module is only reset on VBAT power on reset, we try to preserve
     * the seconds counter between reboots */
    if (rtt->SR & RTC_SR_TIF_MASK) {
        /* Time Invalid Flag is set, clear TIF by writing TSR */

        /* Stop counter to make TSR writable */
        bit_clear32(&rtt->SR, RTC_SR_TCE_SHIFT);

        rtt->TSR = 0;
    }

    /* Clear the alarm flag TAF by writing a new alarm target to TAR */
    rtt->TAR = 0xffffffff;

    /* Enable RTC interrupts */
    NVIC_SetPriority(RTT_IRQ, RTT_IRQ_PRIO);
    NVIC_EnableIRQ(RTT_IRQ);

    rtt_poweron();
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    RTC_Type *rtt = RTT_DEV;
    rtt_callback.overflow_cb = cb;
    rtt_callback.overflow_arg = arg;
    bit_set32(&rtt->IER, RTC_IER_TOIE_SHIFT);
}

void rtt_clear_overflow_cb(void)
{
    RTC_Type *rtt = RTT_DEV;
    rtt_callback.overflow_cb = NULL;
    rtt_callback.overflow_arg = NULL;
    bit_clear32(&rtt->IER, RTC_IER_TOIE_SHIFT);
}

uint32_t rtt_get_counter(void)
{
    RTC_Type *rtt = RTT_DEV;
    uint32_t t;
    for (int i = 0; i < 3; i++) {
        /* Read twice to make sure we get a stable reading */
        t = rtt->TSR;

        if (t == rtt->TSR) {
            return t;
        }
    }
    /* Fallback if we are not getting stable readings */
    return t;
}

void rtt_set_counter(uint32_t counter)
{
    RTC_Type *rtt = RTT_DEV;

    /* Disable time counter before writing to the timestamp register */
    bit_clear32(&rtt->SR, RTC_SR_TCE_SHIFT);
    rtt->TSR = counter;
    /* Enable when done */
    bit_set32(&rtt->SR, RTC_SR_TCE_SHIFT);
}


void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* The alarm is triggered when TSR matches TAR, and TSR increments. This
     * seem counterintuitive as most users expect the alarm to trigger
     * immediately when the counter becomes equal to the alarm time. */
    RTC_Type *rtt = RTT_DEV;

    /* Disable Timer Alarm Interrupt */
    bit_clear32(&rtt->IER, RTC_IER_TAIE_SHIFT);

    rtt->TAR = alarm - 1;

    rtt_callback.alarm_cb = cb;
    rtt_callback.alarm_arg = arg;

    /* Enable Timer Alarm Interrupt */
    bit_set32(&rtt->IER, RTC_IER_TAIE_SHIFT);
}

uint32_t rtt_get_alarm(void)
{
    RTC_Type *rtt = RTT_DEV;
    return rtt->TAR + 1;
}

void rtt_clear_alarm(void)
{
    RTC_Type *rtt = RTT_DEV;
    /* Disable Timer Alarm Interrupt */
    bit_clear32(&rtt->IER, RTC_IER_TAIE_SHIFT);
}

/* RTC module has independent power suply. We can not really turn it on/off. */

void rtt_poweron(void)
{
    RTC_Type *rtt = RTT_DEV;
    /* Enable Time Counter */
    bit_set32(&rtt->SR, RTC_SR_TCE_SHIFT);
}

void rtt_poweroff(void)
{
    RTC_Type *rtt = RTT_DEV;
    /* Disable Time Counter */
    bit_clear32(&rtt->SR, RTC_SR_TCE_SHIFT);
}

void RTT_ISR(void)
{
    RTC_Type *rtt = RTT_DEV;

    if (rtt->SR & RTC_SR_TAF_MASK) {
        if (rtt_callback.alarm_cb != NULL) {
            /* Disable Timer Alarm Interrupt */
            bit_clear32(&rtt->IER, RTC_IER_TAIE_SHIFT);
            rtt_callback.alarm_cb(rtt_callback.alarm_arg);
        }
    }

    if (rtt->SR & RTC_SR_TOF_MASK) {
        if (rtt_callback.overflow_cb != NULL) {
            rtt_callback.overflow_cb(rtt_callback.overflow_arg);
        }
    }

    cortexm_isr_end();
}
