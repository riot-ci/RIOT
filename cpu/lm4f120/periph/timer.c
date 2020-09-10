/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *               2015 Marc Poulhiès <dkm@kataplop.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file        timer.c
 * @brief       Implementation of the low-level timer driver for the LM4F120
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *              Marc Poulhiès <dkm@kataplop.net>
 */

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"
#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief Struct holding the configuration data
 * @{
 */
typedef struct {
    timer_cb_t cb;          /**< timeout callback */
    void *arg;              /**< argument to the callback */
    unsigned int divisor;   /**< software clock divisor */
} timer_params_t;

static timer_params_t params[TIMER_NUMOF];
/**@}*/

#include "hw_timer.h"

/* enable timer interrupts */
static inline void _irq_enable(tim_t dev);

/* Missing from driverlib */
static inline unsigned long
PRIV_TimerPrescaleSnapshotGet(unsigned long ulbase, unsigned long ultimer) {
    return ((ultimer == TIMER_A) ? HWREG(ulbase + TIMER_O_TAPS) :
            HWREG(ulbase + TIMER_O_TBPS));
}

static inline uint64_t _scaled_to_ll_value(uint32_t uncorrected,
                                           uint32_t divisor)
{
    const uint64_t scaledv = (uint64_t) uncorrected * divisor;
    return scaledv;
}

static inline uint32_t _llvalue_to_scaled_value(uint64_t corrected,
                                                uint32_t divisor)
{
    const uint64_t scaledv = corrected / divisor;
    return scaledv;
}

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    if (dev >= TIMER_NUMOF){
        return -1;
    }
    const timer_conf_t *cfg = &timer_config[dev];

    params[dev].cb = cb;
    params[dev].arg = arg;
    params[dev].divisor = ROM_SysCtlClockGet() / freq;

    unsigned int timer_side = TIMER_A;
    unsigned int timer_cfg = TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_PERIODIC_UP |
                             TIMER_TAMR_TAMIE;
    unsigned int timer_intbit = TIMER_TIMA_TIMEOUT | TIMER_TIMA_MATCH;

    ROM_SysCtlPeripheralEnable(cfg->sysctl);

    ROM_TimerDisable(cfg->dev, timer_side);
    ROM_TimerConfigure(cfg->dev, timer_cfg);

    uint64_t val_max = _scaled_to_ll_value(cfg->max, params[dev].divisor);

    ROM_TimerPrescaleSet(cfg->dev, timer_side, val_max >> 32);
    ROM_TimerLoadSet(cfg->dev, timer_side, val_max & 0xFFFFFFFF);
    ROM_TimerIntClear(cfg->dev, timer_intbit);

    ROM_TimerIntEnable(cfg->dev, timer_intbit);

    _irq_enable(dev);
    timer_start(dev);

    return 0;
}

int timer_set_absolute(tim_t dev, int channel, unsigned int value)
{
    if (dev >= TIMER_NUMOF || channel >= timer_config[dev].channels) {
        return -1;
    }
    const timer_conf_t *cfg = &timer_config[dev];

    unsigned int timer_side = TIMER_A;
    unsigned long long scaledv;

    ROM_TimerDisable(cfg->dev, timer_side);

    scaledv = _scaled_to_ll_value(value, params[dev].divisor);

    if (scaledv>>32){
        ROM_TimerPrescaleMatchSet(cfg->dev, timer_side, scaledv >> 32);
    }
    else {
        ROM_TimerPrescaleMatchSet(cfg->dev, timer_side, 0);
    }

    ROM_TimerMatchSet(cfg->dev, timer_side, (uint32_t) (scaledv & 0xFFFFFFFF));
    ROM_TimerEnable(cfg->dev, timer_side);

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (dev >= TIMER_NUMOF || channel >= timer_config[dev].channels) {
        return -1;
    }
    ROM_TimerIntClear(timer_config[dev].dev, TIMER_TIMA_TIMEOUT);
    return 0;
}

unsigned int timer_read(tim_t dev)
{
    unsigned int timer_side = TIMER_A;
    unsigned long long high_bits, high_bits_dup;
    unsigned long long low_bits;
    unsigned long long total;
    unsigned int scaled_value;

    if (dev >= TIMER_NUMOF){
        return -1;
    }

    const timer_conf_t *cfg = &timer_config[dev];

    /* handle overflow happening between the 2 register reads */
    do {
      high_bits = ((uint64_t)PRIV_TimerPrescaleSnapshotGet(cfg->dev,
                   timer_side)) << 32;
      low_bits = (uint64_t)ROM_TimerValueGet(cfg->dev, timer_side);
      high_bits_dup = ((uint64_t)PRIV_TimerPrescaleSnapshotGet(cfg->dev,
                       timer_side)) << 32;
    } while (high_bits != high_bits_dup);

    total = high_bits + low_bits;
    DEBUG("Combined %lx:%lx\n", (uint32_t)(total>>32),
                                (uint32_t)(total & 0xFFFFFFFF));

    scaled_value = _llvalue_to_scaled_value(total, params[dev].divisor);

    return scaled_value;
}

void timer_start(tim_t dev)
{
    if (dev >= TIMER_NUMOF){
        return;
    }

    ROM_TimerEnable(timer_config[dev].dev, TIMER_A);
}

void timer_stop(tim_t dev)
{
    if (dev >= TIMER_NUMOF){
        return;
    }

    ROM_TimerDisable(timer_config[dev].dev, TIMER_A);
}

static inline void _irq_enable(tim_t dev)
{
    if (dev >= TIMER_NUMOF){
        return;
    }

    ROM_IntPrioritySet(timer_config[dev].intbase, 32);
    ROM_IntEnable(timer_config[dev].intbase);
}

void _isr_timer(tim_t dev)
{
    /* Clears both IT */
    ROM_TimerIntClear(timer_config[dev].dev,
                      TIMER_TIMA_TIMEOUT | TIMER_TIMA_MATCH);
    params[dev].cb(params[dev].arg, 0);
    cortexm_isr_end();
}

#if TIMER_0_ISR
void isr_wtimer0a(void)
{
    _isr_timer(0);
}
#endif /* TIMER_0_ISR */

#if TIMER_1_ISR
void isr_wtimer1a(void)
{
    _isr_timer(1);
}
#endif /* TIMER_1_ISR */
/** @} */
