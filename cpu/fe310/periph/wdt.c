/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @ingroup     drivers_periph_wdt
 * @{
 *
 * @file
 * @brief       Implementation of the watchdog peripheral interface
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include "cpu.h"
#include "irq.h"
#include "timex.h"

#include "periph/pm.h"
#include "periph/wdt.h"

#include "vendor/aon.h"
#include "vendor/platform.h"
#include "vendor/plic.h"
#include "vendor/plic_driver.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#ifdef MODULE_PERIPH_WDT_CB
static wdt_cb_t wdt_cb;
static void *wdt_arg;
#endif

void wdt_start(void)
{
    DEBUG("[wdt] start watchdog\n");

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) |= AON_WDOGCFG_ENCOREAWAKE;
}

void wdt_stop(void)
{
    DEBUG("[wdt] stop watchdog\n");

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) &= ~(AON_WDOGCFG_ENCOREAWAKE);
}

void wdt_kick(void)
{
    DEBUG("[wdt] reload the watchdog\n");

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGFEED) = AON_WDOGFEED_VALUE;
}

static inline uint8_t _scale(uint32_t count)
{
    uint8_t scale = 0;
    while (count > (UINT16_MAX - 1)) {
        count >>= 1;
        scale++;
    }

    return scale;
}

static inline uint8_t _setup(uint32_t min_time, uint32_t max_time)
{
    (void)min_time;

    /* Windowed wdt not supported */
    assert(min_time == 0);

    /* Check reset time limit */
    assert((max_time > NWDT_TIME_LOWER_LIMIT) || \
           (max_time < NWDT_TIME_UPPER_LIMIT));

    uint32_t count = ((uint32_t)max_time * RTC_FREQ) / MS_PER_SEC;
    uint8_t scale = _scale(count);

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCMP) = count;

    return scale;
}

void wdt_setup_reboot(uint32_t min_time, uint32_t max_time)
{
    uint8_t scale = _setup(min_time, max_time);

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) = AON_WDOGCFG_RSTEN | AON_WDOGCFG_ZEROCMP | scale;

    DEBUG("[wdt] watchdog setup complete\n");
}

#ifdef MODULE_PERIPH_WDT_CB
static bool wdt_ignore_first_isr = true;

void isr_wdt(int num)
{
    (void)num;

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) &= ~(AON_WDOGCFG_CMPIP);

    if (wdt_ignore_first_isr) {
        wdt_ignore_first_isr = false;
        return;
    }

    wdt_cb(wdt_arg);
    pm_reboot();
    while (1) {}
}

void wdt_setup_reboot_with_callback(uint32_t min_time, uint32_t max_time,
                                    wdt_cb_t cb, void* arg)
{
    uint8_t scale = _setup(min_time, max_time);

    wdt_cb = cb;
    wdt_arg = arg;

    /* disable interrupt */
    PLIC_disable_interrupt(INT_WDOGCMP);

    AON_REG(AON_WDOGKEY) = AON_WDOGKEY_VALUE;
    AON_REG(AON_WDOGCFG) = AON_WDOGCFG_ZEROCMP | scale;

    if (cb) {
        /* enable interrupt */
        set_external_isr_cb(INT_WDOGCMP, isr_wdt);
        PLIC_enable_interrupt(INT_WDOGCMP);
        PLIC_set_priority(INT_WDOGCMP, WDT_INTR_PRIORITY);
    }
}
#endif /* MODULE_PERIPH_WDT_CB */
