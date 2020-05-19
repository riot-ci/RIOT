/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the "sctimer" bsp module
 *
 * The `sctimer` ("single compare timer") in OpenWSN is the lowest timer
 * abstraction which is used by the higher layer timer module `opentimers`. In
 * the end it is responsible for scheduling on the MAC layer. To enable low power
 * energy modes, this timer usually uses the RTC (real time clock) or RTT (real
 * time timer) module.
 *
 * In order to get the most portable code, this implementation uses ztimer and
 * defines a new `ztimer_clock` (ZTIMER_32768) that operates at 32768Khz to have
 * a resolution of ~30usec/tick (same as OpenWSN).
 *
 * When available ZTIMER_32768 will be built on top of `periph_rtt` to get low
 * power capabilities. If not it will be built on top of a regular timer. In
 * either case it will be shifted up if the base frequency is lower than 32768Hz
 * or frac if higher.
 *
 * The `sctimer` is responsible to set the next interrupt. Under circumstances,
 * it may happen, that the next interrupt to schedule is already late, compared
 * to the current time. In this case, timer implementations in OpenWSN directly
 * trigger a hardware interrupt. Until able to trigger sw isr directly we set
 * the callback 0 ticks in the future, which internally will be set to `now + 2`.
 *
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>, April 2019
 *
 * @}
 */
#include "sctimer.h"
#include "debugpins.h"

#include "ztimer.h"
#include "ztimer/convert.h"
#include "ztimer/convert_frac.h"
#include "ztimer/convert_shift.h"
#include "ztimer/config.h"

#include "periph_conf.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

/**
 * @brief   Maximum counter difference to not consider an ISR late, this should
 *          account for the largest timer interval OpenWSN scheduler might work
 *          with. When running only the stack this should not be more than
 *          SLOT_DURATION, but when using cjoin it is 65535ms
 */
#ifndef SCTIMER_LOOP_THRESHOLD
#define SCTIMER_LOOP_THRESHOLD       (2 * PORT_TICS_PER_MS * 65535)
#endif

#if CONFIG_ZTIMER_MSEC_BASE_FREQ > 32768U
static ztimer_convert_frac_t _ztimer_convert_frac_32768;
#define ZTIMER_32768_CONVERT_LOWER_FREQ    CONFIG_ZTIMER_MSEC_BASE_FREQ
#define ZTIMER_32768_CONVERT_LOWER         (ZTIMER_MSEC_BASE)
/* cppcheck-suppress preprocessorErrorDirective
 * (reason: cppcheck fails to see that CONFIG_ZTIMER_MSEC_BASE_FREQ
 * is set in ztimer/config.h to a non zero value */
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
      ((32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ) == 0)
static ztimer_convert_shift_t _ztimer_convert_shift_32768;
#define ZTIMER_32768_CONVERT_HIGHER_FREQ   CONFIG_ZTIMER_MSEC_BASE_FREQ
#define ZTIMER_32768_CONVERT_HIGHER        (ZTIMER_MSEC_BASE)
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
      ((32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ) != 0)
#error No suitable ZTIMER_MSEC_BASE config. Maybe add USEMODULE += ztimer_usec?
#endif

static sctimer_cbt sctimer_cb;
static ztimer_t ztimer_sctimer;
static ztimer_clock_t *ZTIMER_32768 = NULL;

static void sctimer_isr_internal(void *arg)
{
    (void)arg;

    if (sctimer_cb != NULL) {
        debugpins_isr_set();
        sctimer_cb();
        debugpins_isr_clr();
    }
}

void sctimer_init(void)
{
#if CONFIG_ZTIMER_MSEC_BASE_FREQ > 32768U
    ZTIMER_32768 = &_ztimer_convert_frac_32768.super.super;
/* cppcheck-suppress preprocessorErrorDirective
 * (reason: cppcheck fails to see that CONFIG_ZTIMER_MSEC_BASE_FREQ
 * is set in ztimer/config.h to a non zero value */
#elif (CONFIG_ZTIMER_MSEC_BASE_FREQ < 32768U) && \
      (32768U % CONFIG_ZTIMER_MSEC_BASE_FREQ == 0)
    ZTIMER_32768 = &_ztimer_convert_shift_32768.super.super;
#elif CONFIG_ZTIMER_MSEC_BASE_FREQ == 32768U
    ZTIMER_32768 = ZTIMER_MSEC_BASE;
#else
#error \
    No suitable ZTIMER_MSEC_BASE_FREQ config. Maybe add USEMODULE += ztimer_usec?
#endif

#if defined(ZTIMER_32768_CONVERT_LOWER)
    LOG_DEBUG("[sctimer]: ZTIMER_32768 convert_frac from %lu to 32768\n",
              (long unsigned)ZTIMER_32768_CONVERT_LOWER_FREQ);
    ztimer_convert_frac_init(&_ztimer_convert_frac_32768,
                             ZTIMER_32768_CONVERT_LOWER,
                             SCTIMER_FREQUENCY,
                             ZTIMER_32768_CONVERT_LOWER_FREQ);
#elif defined(ZTIMER_32768_CONVERT_HIGHER)
    LOG_DEBUG("[sctimer]: ZTIMER_32768 convert_shift %lu to 32768\n",
              (long unsigned)ZTIMER_32768_CONVERT_HIGHER_FREQ);
    ztimer_convert_shift_up_init(&_ztimer_convert_shift_32768,
                                 ZTIMER_32768_CONVERT_HIGHER,
                                 __builtin_ctz(SCTIMER_FREQUENCY /
                                               CONFIG_ZTIMER_MSEC_BASE_FREQ));
#endif

    ztimer_sctimer.callback = sctimer_isr_internal;
}

void sctimer_set_callback(sctimer_cbt cb)
{
    sctimer_cb = cb;
}

void sctimer_setCompare(uint32_t val)
{
    unsigned state = irq_disable();

    uint32_t now = ztimer_now(ZTIMER_32768);

    /* if the next compare value (isr) to schedule is already later than the
       required value, but close enough to think we have been slow in scheduling
       it, trigger the ISR right away */
    if (now - val < SCTIMER_LOOP_THRESHOLD && now > val) {
        ztimer_set(ZTIMER_32768, &ztimer_sctimer, 0);
    }
    else {
        ztimer_set(ZTIMER_32768, &ztimer_sctimer, val - now);
    }

    LOG_DEBUG("[sctimer]: set callback to %" PRIu32 " at %" PRIu32 "\n", val,
              now);

    irq_restore(state);
}

uint32_t sctimer_readCounter(void)
{
    uint32_t now = ztimer_now(ZTIMER_32768);

    LOG_DEBUG("[sctimer]: now %" PRIu32 "\n", now);
    return now;
}

void sctimer_enable(void)
{
    /* not supported, sctimer does not control RTT or ztimer */
}

void sctimer_disable(void)
{
    /* not supported, sctimer does not control RTT or ztimer */
}
