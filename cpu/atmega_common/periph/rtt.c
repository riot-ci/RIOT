/*
 * Copyright (C) 2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @ingroup cpu_atmega_common
 * @ingroup drivers_periph_rtt
 * @{
 *
 * @file
 * @brief       Low-level ATmega RTT driver implementation
 *
 * @note        The RTT only works if the board is equipped with a 32kHz
 *              oscillator.
 *
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 * @author      Alexander Chudov <chudov@gmail.com>
 *
 * For all atmega except rfr2
 * In order to safely sleep when using the RTT:
 * 1. Disable interrupts
 * 2. Write to one of the asynch registers (e.g. TCCR2A)
 * 3. Wait for ASSR register's busy flags to clear
 * 4. Re-enable interrupts
 * 5. Sleep before interrupt re-enable takes effect
 *
 * For MCUs with MAC symbol counter (atmega*rfr2):
 * The MAC symbol counter is automatically switch to TOSC1 32.768kHz clock
 * when transceiver or CPU is going to sleep. The MAC symbol counter is
 * sourced by 62.500 kHz derived from 32.768kHz TOSC1 or 16 MHz system clock
 * For current implementation symbol counter is always use 32.768kHz TOSC1 clock
 * For alarms the SCOCR2 register is used.
 *
 * SCCR0 is defined if an MCU has MAC symbol counter
 *
 * @}
 */

#include <avr/interrupt.h>

#ifdef SCCR0
#define RTT_BACKEND_SC  (1)
#include "byteorder.h"
#endif
#include "cpu.h"
#include "irq.h"
#include "periph/rtt.h"
#include "periph_conf.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef RTT_BACKEND_SC
/**
 * Atomic read of 32-bit registers according
 * to section 10.3 of the Datasheet
 * @{
 */

__attribute__((always_inline))
  static inline uint32_t rg_read32(volatile uint8_t *hh,
                                   volatile uint8_t *hl,
                                   volatile uint8_t *lh,
                                   volatile uint8_t *ll)
{
    le_uint32_t rg;

    rg.u8[0] = *ll;
    rg.u8[1] = *lh;
    rg.u8[2] = *hl;
    rg.u8[3] = *hh;

    return rg.u32;
}

__attribute__((always_inline))
  static inline void rg_write32(uint32_t val, volatile uint8_t *hh,
                                              volatile uint8_t *hl,
                                              volatile uint8_t *lh,
                                              volatile uint8_t *ll)
{
    le_uint32_t rg;

    rg.u32 = val;

    *hh = rg.u8[3];
    *hl = rg.u8[2];
    *lh = rg.u8[1];
    *ll = rg.u8[0];
}

/* To build proper register names */
#ifndef CONCAT
#define CONCAT(a, b) (a##b)
#endif

/* To read the whole 32-bit register */
#define RG_READ32(reg)  (rg_read32(&CONCAT(reg, HH), \
                                   &CONCAT(reg, HL), \
                                   &CONCAT(reg, LH), \
                                   &CONCAT(reg, LL)))

/* To write the whole 32-bit register */
#define RG_WRITE32(reg, val)  (rg_write32(val, &CONCAT(reg, HH), \
                                               &CONCAT(reg, HL), \
                                               &CONCAT(reg, LH), \
                                               &CONCAT(reg, LL)))

/** @} */
#endif

typedef struct {
#ifndef RTT_BACKEND_SC
    uint16_t ext_comp;          /* Extend compare to 24-bits */
#endif
    rtt_cb_t alarm_cb;          /* callback called from RTT alarm */
    void *alarm_arg;            /* argument passed to the callback */
    rtt_cb_t overflow_cb;       /* callback called when RTT overflows */
    void *overflow_arg;         /* argument passed to the callback */
} rtt_state_t;

static rtt_state_t rtt_state;
#ifndef RTT_BACKEND_SC
static uint16_t ext_cnt;
#endif

static inline void _asynch_wait(void)
{
#ifdef RTT_BACKEND_SC
    /* Wait until counter update flag clear. */
    while (SCSR & ((1 << SCBSY) )) {}
#else
    /* Wait until all busy flags clear. According to the datasheet,
     * this can take up to 2 positive edges of TOSC1 (32kHz). */
    while (ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB)
                | (1 << TCR2AUB) | (1 << TCR2BUB))) {}
#endif
}

/* interrupts are disabled here */
static uint32_t _safe_cnt_get(void)
{
#ifdef RTT_BACKEND_SC
    return RG_READ32(SCCNT);
#else
    uint8_t cnt = TCNT2;

    /* If an overflow occurred since we disabled interrupts, manually
     * increment `ext_cnt`
     */
    if (TIFR2 & (1 << TOV2)) {
        ++ext_cnt;

        /* If an overflow occurred just after we read `TCNT2`
           it has overflown back to zero now */
        if (cnt == 255) {
            cnt = 0;
        }

        /* Clear interrupt flag */
        TIFR2 = (1 << TOV2);
    }

    return (ext_cnt << 8) | cnt;
#endif
}

#ifdef RTT_BACKEND_SC
static inline void _timer_init(void)
{
    /*
     * ATmega256RFR2 symbol counter init sequence:
     * 1. Disable all related interrupts
     * 2. Enable 32 kHz oscillator
     * 3. Enable symbol counter, clock it from TOSC1 only
     * 4. Reset prescaller, enable rx timestamping, start symbol counter
     */

    /* Disable all symbol counter interrupts */
    SCIRQM = 0;

    /* Clear all interrupt flags by writing '1' */
    SCIRQS = (1 << IRQSBO) | (1 << IRQSOF)
            | (1 << IRQSCP3) | (1 << IRQSCP2) | (1 << IRQSCP1);

    /* Reset compare values */
    RG_WRITE32(SCOCR1, 0);
    RG_WRITE32(SCOCR2, 0);
    RG_WRITE32(SCOCR3, 0);

    /* Enable 32 kHz oscillator. All T/C2-related settings are overridden */
    ASSR = (1 << AS2);

    /* Enable symbol counter, clock from TOSC1, timestamping enabled */
    SCCR0 = (1 << SCRES) | (1 << SCEN) | (1 << SCCKSEL) | (1 << SCTSE);

    /* Reset the symbol counter */
    RG_WRITE32(SCCNT, 0);
    /* Wait until not busy anymore */
    DEBUG("RTT waits until SC not busy\n");
}
#else
static inline uint8_t _rtt_div(uint16_t freq)
{
    switch (freq) {
    case 32768: return 0x1;
    case  4096: return 0x2;
    case  1024: return 0x3;
    case   512: return 0x4;
    case   256: return 0x5;
    case   128: return 0x6;
    case    32: return 0x7;
    default   : assert(0);
    }
}

static inline void _timer_init(void)
{

    /*
     * From the datasheet section "Asynchronous Operation of Timer/Counter2"
     * p148 for ATmega1284P.
     * 1. Disable the Timer/Counter2 interrupts by clearing OCIE2x and TOIE2.
     * 2. Select clock source by setting AS2 as appropriate.
     * 3. Write new values to TCNT2, OCR2x, and TCCR2x.
     * 4. To switch to asynchronous: Wait for TCN2UB, OCR2xUB, TCR2xUB.
     * 5. Clear the Timer/Counter2 Interrupt Flags.
     * 6. Enable interrupts, if needed
     */

    /* Disable all timer 2 interrupts */
    TIMSK2 = 0;

    /* Select asynchronous clock source */
    ASSR = (1 << AS2);

    /* Set counter to 0 */
    TCNT2 = 0;

    /* Reset compare values */
    OCR2A = 0;
    OCR2B = 0;

    /* Reset timer control */
    TCCR2A = 0;

    /* 32768Hz / n */
    TCCR2B = _rtt_div(RTT_FREQUENCY);

    /* Wait until not busy anymore */
    DEBUG("RTT waits until ASSR not busy\n");
}
#endif

void rtt_init(void)
{
    DEBUG("Initializing RTT\n");

    rtt_poweron();

    _timer_init();
    _asynch_wait();

#ifndef RTT_BACKEND_SC
    /* Clear interrupt flags */
    /* Oddly, this is done by writing ones; see datasheet */
    TIFR2 = (1 << OCF2B) | (1 << OCF2A) | (1 << TOV2);

    /* Enable 8-bit overflow interrupt */
    TIMSK2 |= (1 << TOIE2);
#endif
    DEBUG("RTT initialized\n");
}

void rtt_set_overflow_cb(rtt_cb_t cb, void *arg)
{
    /* Make non-atomic write to callback atomic */
    unsigned state = irq_disable();

    rtt_state.overflow_cb = cb;
    rtt_state.overflow_arg = arg;

    irq_restore(state);
}

void rtt_clear_overflow_cb(void)
{
    /* Make non-atomic write to callback atomic */
    unsigned state = irq_disable();

    rtt_state.overflow_cb = NULL;
    rtt_state.overflow_arg = NULL;

    irq_restore(state);
}

uint32_t rtt_get_counter(void)
{
    unsigned state;
    uint32_t now;
#ifndef RTT_BACKEND_SC
    /* Make sure it is safe to read TCNT2, in case we just woke up */
    DEBUG("RTT sleeps until safe to read TCNT2\n");
    TCCR2A = 0;
    _asynch_wait();
#endif
    state = irq_disable();
    now = _safe_cnt_get();
    irq_restore(state);

    return now;
}

void rtt_set_counter(uint32_t counter)
{
    /* Wait until not busy anymore (should be immediate) */
    DEBUG("RTT sleeps until safe to write\n");
    _asynch_wait();

    /* Make non-atomic writes atomic (for concurrent access) */
    unsigned state = irq_disable();
#ifdef RTT_BACKEND_SC
    /* Clear overflow flag by writing a one; see datasheet */
    SCIRQS = (1 << IRQSOF);

    RG_WRITE32(SCCNT, counter);
    _asynch_wait();
#else
    /* Prevent overflow flag from being set during update */
    TCNT2 = 0;

    /* Clear overflow flag */
    /* Oddly, this is done by writing a one; see datasheet */
    TIFR2 = 1 << TOV2;

    ext_cnt = (uint16_t)(counter >> 8);
    TCNT2 = (uint8_t)counter;
#endif
    irq_restore(state);
}

void rtt_set_alarm(uint32_t alarm, rtt_cb_t cb, void *arg)
{
    /* Disable alarm */
    rtt_clear_alarm();
#ifdef RTT_BACKEND_SC
    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    /* Set the alarm value to SCOCR2. Atomic for concurrent access */
    RG_WRITE32(SCOCR2, alarm);

    rtt_state.alarm_cb = cb;
    rtt_state.alarm_arg = arg;

    irq_restore(state);

    DEBUG("RTT set alarm SCCNT: %" PRIu32 ", SCOCR2: %" PRIu32 "\n",
            RG_READ32(SCCNT), RG_READ32(SCOCR2));

    /* Enable alarm interrupt */
    SCIRQS |= (1 << IRQSCP2);
    SCIRQM |= (1 << IRQMCP2);

    DEBUG("RTT alarm interrupt active\n");

#else
    /* Make sure it is safe to read TCNT2, in case we just woke up, and */
    /* safe to write OCR2B (in case it was busy) */
    DEBUG("RTT sleeps until safe read TCNT2 and to write OCR2B\n");
    TCCR2A = 0;
    _asynch_wait();
    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    uint32_t now = _safe_cnt_get();

    /* Set the alarm value. Atomic for concurrent access */
    rtt_state.ext_comp = (uint16_t)(alarm >> 8);
    OCR2A = (uint8_t)alarm;

    rtt_state.alarm_cb = cb;
    rtt_state.alarm_arg = arg;

    irq_restore(state);

    DEBUG("RTT set alarm TCNT2: %" PRIu8 ", OCR2A: %" PRIu8 "\n", TCNT2, OCR2A);

    /* Enable alarm interrupt only if it will trigger before overflow */
    if (rtt_state.ext_comp <= (uint16_t)(now >> 8)) {
        /* Clear interrupt flag */
        TIFR2 = (1 << OCF2A);

        /* Enable interrupt */
        TIMSK2 |= (1 << OCIE2A);

        DEBUG("RTT alarm interrupt active\n");
    }
    else {
        DEBUG("RTT alarm interrupt not active\n");
    }

#endif
}

uint32_t rtt_get_alarm(void)
{
#ifdef RTT_BACKEND_SC
    return RG_READ32(SCOCR2);
#else
    return (rtt_state.ext_comp << 8) | OCR2A;
#endif
}

void rtt_clear_alarm(void)
{
    /* Make non-atomic writes atomic */
    unsigned state = irq_disable();

    /* Disable alarm interrupt */
#ifdef RTT_BACKEND_SC
    SCIRQM &= ~(1 << IRQMCP2);
#else
    TIMSK2 &= ~(1 << OCIE2A);
#endif
    rtt_state.alarm_cb = NULL;
    rtt_state.alarm_arg = NULL;

    irq_restore(state);
}

void rtt_poweron(void)
{
#ifdef RTT_BACKEND_SC
    SCCR0 |= (1 << SCEN);
#else
    power_timer2_enable();
#endif
}

void rtt_poweroff(void)
{
#ifdef RTT_BACKEND_SC
    SCCR0 &= ~(1 << SCEN);
#else
    power_timer2_disable();
#endif
}

#ifdef RTT_BACKEND_SC
ISR(SCNT_OVFL_vect)
{
    atmega_enter_isr();
    /* Execute callback */
    if (rtt_state.overflow_cb != NULL) {
        rtt_state.overflow_cb(rtt_state.overflow_arg);
    }

    atmega_exit_isr();
}
#else
ISR(TIMER2_OVF_vect)
{
    atmega_enter_isr();

    ext_cnt++;

    /* Enable RTT alarm if overflowed enough times */
    if (rtt_state.ext_comp <= ext_cnt) {
        /* Clear interrupt flag */
        TIFR2 = (1 << OCF2A);

        /* Enable interrupt */
        TIMSK2 |= (1 << OCIE2A);
    }

    /* Virtual 24-bit timer overflowed */
    if (ext_cnt == 0) {
        /* Execute callback */
        if (rtt_state.overflow_cb != NULL) {
            rtt_state.overflow_cb(rtt_state.overflow_arg);
        }
    }

    atmega_exit_isr();
}
#endif

#ifdef RTT_BACKEND_SC
ISR(SCNT_CMP2_vect)
#else
ISR(TIMER2_COMPA_vect)
#endif
{
    atmega_enter_isr();
    /* Disable alarm interrupt */
#ifdef RTT_BACKEND_SC
    SCIRQM &= ~(1 << IRQMCP2);
#else
    TIMSK2 &= ~(1 << OCIE2A);
#endif
    if (rtt_state.alarm_cb != NULL) {
        /* Clear callback */
        rtt_cb_t cb = rtt_state.alarm_cb;
        rtt_state.alarm_cb = NULL;

        /* Execute callback */
        cb(rtt_state.alarm_arg);
    }

    atmega_exit_isr();
}
