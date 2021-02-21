/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atxmega
 * @ingroup     cpu_atxmega_periph
 * @{
 *
 * @file
 * @brief       Low-level TIMER driver implementation
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include <avr/interrupt.h>

#include "cpu.h"
#include "thread.h"

#include "periph/timer.h"

#include "board.h"
#include "periph_conf.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   We have 7 possible prescaler values
 */
#define PRESCALE_NUMOF      (7U)

/**
 * @brief   Possible prescaler values, encoded as 2 ^ val
 */
static const uint8_t prescalers[] = { 0, 1, 2, 3, 6, 8, 10 };

/**
 * @brief   Timer state context
 */
typedef struct {
    timer_cb_t cb;          /**< interrupt callback */
    void *arg;              /**< interrupt callback argument */
    uint8_t prescaler;      /**< remember the prescaler value */
    uint8_t channels;       /**< number of channels */
} ctx_t;

/**
 * @brief   Allocate memory for saving the device states
 * @{
 */
#ifdef TIMER_NUMOF
static ctx_t ctx[TIMER_NUMOF] = { { NULL } };
#else
/* fallback if no timer is configured */
static ctx_t *ctx[] = { { NULL } };
#endif
/** @} */

#if TIMER_CH_MAX_NUMOF * TIMER_NUMOF > 32
#error "periph_timer: The current implementation supports at most 32 / TIMER_CH_MAX_NUMOF timers"
#endif

static uint32_t _oneshot;

static inline void set_oneshot(tim_t tim, int chan)
{
    _oneshot |= (1 << chan) << (TIMER_CH_MAX_NUMOF * tim);
}

static inline void clear_oneshot(tim_t tim, int chan)
{
    _oneshot &= ~((1 << chan) << (TIMER_CH_MAX_NUMOF * tim));
}

static inline bool is_oneshot(tim_t tim, int chan)
{
    return _oneshot & ((1 << chan) << (TIMER_CH_MAX_NUMOF * tim));
}

/**
 * @brief Setup the given timer
 */
int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer.c: freq = %ld, Core Clock = %ld\n", freq, CLOCK_CORECLOCK);

    TC0_t *dev;
    uint8_t pre;
    uint8_t ch;

    /* make sure given device is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* figure out if freq is applicable */
    for (pre = 0; pre < PRESCALE_NUMOF; pre++) {
        if ((CLOCK_CORECLOCK >> prescalers[pre]) == freq) {
            break;
        }
    }
    if (pre == PRESCALE_NUMOF) {
        DEBUG("timer.c: prescaling failed!\n");
        return -1;
    }

    dev = timer_config[tim].dev;

    /* stop and reset timer */
    dev->CTRLA = 0;                    /* Stop */
    dev->CTRLFSET = TC_CMD_RESET_gc;   /* Force Reset */

    /* save interrupt context and timer Prescaler */
    ctx[tim].cb = cb;
    ctx[tim].arg = arg;
    ctx[tim].prescaler = (0x07 & (pre + 1));

    /* Check enabled channels */
    ctx[tim].channels = 0;
    for (ch = 0; ch < 4; ch++) {
        if (timer_config[tim].int_lvl[ch] != CPU_INT_LVL_OFF) {
            ctx[tim].channels++;
        }
    }

    if (timer_config[tim].type != TC_TYPE_0
        && timer_config[tim].type != TC_TYPE_4) {
        if (ctx[tim].channels > 2) {
            DEBUG("timer.c: wrong number of channels. max value is 2.\n");
            return -1;
        }
    }

    if (timer_config[tim].type == TC_TYPE_2
        || timer_config[tim].type == TC_TYPE_4
        || timer_config[tim].type == TC_TYPE_5) {
        DEBUG("timer.c: Timer version %d is current not supported.\n",
            timer_config[tim].type);
        return -1;
    }

    /* Normal Counter with rollover */
    dev->CTRLB = TC_WGMODE_NORMAL_gc;

    /* Compare or Capture Interrupt Level */
    dev->INTCTRLB  = (timer_config[tim].int_lvl[0] << TC0_CCAINTLVL_gp);
    dev->INTCTRLB |= (timer_config[tim].int_lvl[1] << TC0_CCBINTLVL_gp);
    if (ctx[tim].channels > 2) {
        dev->INTCTRLB |= (timer_config[tim].int_lvl[2] << TC0_CCCINTLVL_gp);
        dev->INTCTRLB |= (timer_config[tim].int_lvl[3] << TC0_CCDINTLVL_gp);
    }

    /* Free running counter */
    dev->PER = 0xFFFF;

    DEBUG("timer.c: prescaler set to %d \n", ctx[tim].prescaler);
    dev->CTRLA = ctx[tim].prescaler;

    return 0;
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    TC0_t *dev;

    if (channel >= ctx[tim].channels) {
        return -1;
    }

    DEBUG("Setting timer %i channel %i to %04x\n", tim, channel, value);

    set_oneshot(tim, channel);

    dev = timer_config[tim].dev;

    /* Compare or Capture Disable */
    dev->CTRLB &= ~(1 << (channel + TC0_CCAEN_bp));

    /* Clear Interrupt Flag */
    dev->INTFLAGS &= ~(1 << (channel + TC0_CCAIF_bp));

    /* set value to compare with rollover */
    *(((uint16_t *)(&dev->CCA)) + channel) = (dev->CNT + (uint16_t)value);

    /* Compare or Capture Enable */
    dev->CTRLB |= (1 << (channel + TC0_CCAEN_bp));

    return 1;
}

int timer_set_periodic(tim_t tim, int channel, unsigned int value, uint8_t flags)
{
    TC0_t *dev;

    if (channel > 0 || ctx[tim].channels != 1) {
        DEBUG("Only channel 0 can be set as periodic and channels must be 1\n");

        return -1;
    }

    DEBUG("Setting timer %i channel 0 to %i and flags %i (repeating)\n",
          tim, value, flags);

    dev = timer_config[tim].dev;

    /* Set Freq. Mode  (TIM_FLAG_RESET_ON_MATCH) */
    dev->CTRLB = TC_WGMODE_FRQ_gc;

    if (flags & TIM_FLAG_RESET_ON_SET) {
        dev->CTRLFSET = TC_CMD_RESTART_gc;
    }

    /* Clear Interrupt Flag */
    dev->INTFLAGS &= ~(1 << (channel + TC0_CCAIF_bp));

    /* set value to compare match */
    *(((uint16_t *)(&dev->CCA)) + channel) = (uint16_t)value;

    clear_oneshot(tim, channel);

    /* Compare or Capture Enable */
    dev->CTRLB |= (1 << (channel + TC0_CCAEN_bp));

    return 0;
}

int timer_clear(tim_t tim, int channel)
{
    TC0_t *dev;

    if (channel >= ctx[tim].channels) {
        return -1;
    }

    DEBUG("timer_clear channel %d\n", channel  );

    dev = timer_config[tim].dev;

    /* Compare or Capture Disable */
    dev->CTRLB &= ~(1 << (channel + TC0_CCAEN_bp));

    /* Clear Interrupt Flag
     * The CCxIF is automatically cleared when the corresponding
     * interrupt vector is executed.*/
    dev->INTFLAGS &= ~(1 << (channel + TC0_CCAIF_bp));

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    DEBUG("timer_read\n");
    return (unsigned int)timer_config[tim].dev->CNT;
}

void timer_stop(tim_t tim)
{
    DEBUG("timer_stop\n");
    timer_config[tim].dev->CTRLA = 0;
}

void timer_start(tim_t tim)
{
    DEBUG("timer_start\n");
    timer_config[tim].dev->CTRLA = ctx[tim].prescaler;
}

#ifdef TIMER_NUMOF
static inline void _isr(tim_t tim, int channel)
{
    avr8_enter_isr();

    DEBUG("timer _isr channel %d\n", channel);

    if (is_oneshot(tim, channel)) {
        timer_config[tim].dev->CTRLB &= ~(1 << (channel + TC0_CCAEN_bp));
    }

    if (ctx[tim].cb) {
        ctx[tim].cb(ctx[tim].arg, channel);
    }

    avr8_exit_isr();
}
#endif

#ifdef TIMER_0_ISRA
ISR(TIMER_0_ISRA, ISR_BLOCK)
{
    _isr(0, 0);
}
#endif
#ifdef TIMER_0_ISRB
ISR(TIMER_0_ISRB, ISR_BLOCK)
{
    _isr(0, 1);
}
#endif
#ifdef TIMER_0_ISRC
ISR(TIMER_0_ISRC, ISR_BLOCK)
{
    _isr(0, 2);
}
#endif
#ifdef TIMER_0_ISRD
ISR(TIMER_0_ISRD, ISR_BLOCK)
{
    _isr(0, 3);
}
#endif /* TIMER_0 */

#ifdef TIMER_1_ISRA
ISR(TIMER_1_ISRA, ISR_BLOCK)
{
    _isr(1, 0);
}
#endif
#ifdef TIMER_1_ISRB
ISR(TIMER_1_ISRB, ISR_BLOCK)
{
    _isr(1, 1);
}
#endif
#ifdef TIMER_1_ISRC
ISR(TIMER_1_ISRC, ISR_BLOCK)
{
    _isr(1, 2);
}
#endif
#ifdef TIMER_1_ISRD
ISR(TIMER_1_ISRD, ISR_BLOCK)
{
    _isr(1, 3);
}
#endif /* TIMER_1 */

#ifdef TIMER_2_ISRA
ISR(TIMER_2_ISRA, ISR_BLOCK)
{
    _isr(2, 0);
}
#endif
#ifdef TIMER_2_ISRB
ISR(TIMER_2_ISRB, ISR_BLOCK)
{
    _isr(2, 1);
}
#endif
#ifdef TIMER_2_ISRC
ISR(TIMER_2_ISRC, ISR_BLOCK)
{
    _isr(2, 2);
}
#endif
#ifdef TIMER_2_ISRD
ISR(TIMER_2_ISRD, ISR_BLOCK)
{
    _isr(2, 3);
}
#endif /* TIMER_2 */

#ifdef TIMER_3_ISRA
ISR(TIMER_3_ISRA, ISR_BLOCK)
{
    _isr(3, 0);
}
#endif
#ifdef TIMER_3_ISRB
ISR(TIMER_3_ISRB, ISR_BLOCK)
{
    _isr(3, 1);
}
#endif
#ifdef TIMER_3_ISRC
ISR(TIMER_3_ISRC, ISR_BLOCK)
{
    _isr(3, 2);
}
#endif
#ifdef TIMER_3_ISRD
ISR(TIMER_3_ISRD, ISR_BLOCK)
{
    _isr(3, 3);
}
#endif /* TIMER_3 */

#ifdef TIMER_4_ISRA
ISR(TIMER_4_ISRA, ISR_BLOCK)
{
    _isr(4, 0);
}
#endif
#ifdef TIMER_4_ISRB
ISR(TIMER_4_ISRB, ISR_BLOCK)
{
    _isr(4, 1);
}
#endif
#ifdef TIMER_4_ISRC
ISR(TIMER_4_ISRC, ISR_BLOCK)
{
    _isr(4, 2);
}
#endif
#ifdef TIMER_4_ISRD
ISR(TIMER_4_ISRD, ISR_BLOCK)
{
    _isr(4, 3);
}
#endif /* TIMER_4 */

#ifdef TIMER_5_ISRA
ISR(TIMER_5_ISRA, ISR_BLOCK)
{
    _isr(5, 0);
}
#endif
#ifdef TIMER_5_ISRB
ISR(TIMER_5_ISRB, ISR_BLOCK)
{
    _isr(5, 1);
}
#endif
#ifdef TIMER_5_ISRC
ISR(TIMER_5_ISRC, ISR_BLOCK)
{
    _isr(5, 2);
}
#endif
#ifdef TIMER_5_ISRD
ISR(TIMER_5_ISRD, ISR_BLOCK)
{
    _isr(5, 3);
}
#endif /* TIMER_5 */

#ifdef TIMER_6_ISRA
ISR(TIMER_6_ISRA, ISR_BLOCK)
{
    _isr(6, 0);
}
#endif
#ifdef TIMER_6_ISRB
ISR(TIMER_6_ISRB, ISR_BLOCK)
{
    _isr(6, 1);
}
#endif
#ifdef TIMER_6_ISRC
ISR(TIMER_6_ISRC, ISR_BLOCK)
{
    _isr(6, 2);
}
#endif
#ifdef TIMER_6_ISRD
ISR(TIMER_6_ISRD, ISR_BLOCK)
{
    _isr(6, 3);
}
#endif /* TIMER_6 */

#ifdef TIMER_7_ISRA
ISR(TIMER_7_ISRA, ISR_BLOCK)
{
    _isr(7, 0);
}
#endif
#ifdef TIMER_7_ISRB
ISR(TIMER_7_ISRB, ISR_BLOCK)
{
    _isr(7, 1);
}
#endif
#ifdef TIMER_7_ISRC
ISR(TIMER_7_ISRC, ISR_BLOCK)
{
    _isr(7, 2);
}
#endif
#ifdef TIMER_7_ISRB
ISR(TIMER_7_ISRD, ISR_BLOCK)
{
    _isr(7, 3);
}
#endif /* TIMER_7 */
