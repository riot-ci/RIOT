/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for the CC26x0
 *
 * @author      Leon M. George <leon@georgemail.eu>
 * @author      Sebastian Meiling <s@mlng.net>
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph_conf.h"
#include "periph/timer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define LOAD_VALUE              (0xffff)

#define TIMER_A_IRQ_MASK        (0x000000ff)
#define TIMER_B_IRQ_MASK        (0x0000ff00)

#define TIMER_IRQ_PRIO          (1U)
/**
 * @brief   Allocate memory for the interrupt context
 */
static timer_isr_ctx_t ctx[TIMER_NUMOF];

/**
 * @brief           Get the GPT register base for a timer
 *
 * @param[in] tim   index of the timer
 *
 * @return          base address
 */
static inline gpt_reg_t *dev(tim_t tim)
{
    return timer_config[tim].dev;
}

int timer_init(tim_t tim, unsigned long freq, timer_cb_t cb, void *arg)
{
    DEBUG("timer_init(%u, %lu)\n", tim, freq);
    /* make sure given timer is valid */
    if (tim >= TIMER_NUMOF) {
        return -1;
    }

    /* enable the timer clock */
    PRCM->GPTCLKGR |= (1 << tim);
    PRCM->CLKLOADCTL = CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) {}

    /* disable (and reset) timer */
    dev(tim)->CTL = 0;

    /* save context */
    ctx[tim].cb = cb;
    ctx[tim].arg = arg;

    uint32_t chan_mode = (GPT_TXMR_TXMR_PERIODIC | GPT_TXMR_TXMIE);
    uint32_t prescaler = 0;
    if (timer_config[tim].cfg == GPT_CFG_32T) {
        if (timer_config[tim].chn > 1) {
            DEBUG("timer_init: in 32Bit mode single channel only!\n");
            return -1;
        }
        if (freq != RCOSC48M_FREQ) {
            DEBUG("timer_init: in 32Bit mode freq must equal system clock!\n");
            return -1;
        }
        chan_mode |= GPT_TXMR_TXCDIR_UP;
    }
    else if (timer_config[tim].cfg == GPT_CFG_16T) {
        /* prescaler only available in 16Bit mode */
        prescaler = RCOSC48M_FREQ;
        prescaler += freq / 2;
        prescaler /= freq;
        if (prescaler > 0) {
            prescaler--;
        }
        if (prescaler > 255) {
            prescaler = 255;
        }
        dev(tim)->TAILR = LOAD_VALUE;
        dev(tim)->TAPR = prescaler;
    }
    else {
        DEBUG("timer_init: invalid timer config must be 16 or 32Bit mode!\n");
        return -1;
    }
    /* configure timer to 16-bit, periodic, up-counting */
    dev(tim)->CFG  = timer_config[tim].cfg;
    dev(tim)->CTL = GPT_CTL_TAEN;
    dev(tim)->TAMR = chan_mode;
    /* enable global timer interrupt and start the timer */
    IRQn_Type irqn = GPTIMER_0A_IRQN + (2 * tim);
    NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
    NVIC_EnableIRQ(irqn);
    if (timer_config[tim].chn == 2) {
        /* set the timer speed */
        dev(tim)->TBPR = prescaler;
        dev(tim)->TBMR = chan_mode;
        dev(tim)->TBILR = LOAD_VALUE;
        dev(tim)->CTL = GPT_CTL_TAEN | GPT_CTL_TBEN;
        irqn++;
        NVIC_SetPriority(irqn, TIMER_IRQ_PRIO);
        NVIC_EnableIRQ(irqn);
    }
    return 0;
}

int timer_set(tim_t tim, int channel, unsigned int timeout)
{
    return timer_set_absolute(tim, channel, timer_read(tim) + timeout);
}

int timer_set_absolute(tim_t tim, int channel, unsigned int value)
{
    DEBUG("timer_set_absolute(%u, %u, %u)\n", tim, channel, value);

    if ((tim >= TIMER_NUMOF) || (channel >= timer_config[tim].chn)) {
        return -1;
    }
    switch (channel) {
        case 0:
            dev(tim)->ICLR = GPT_IMR_TAMIM;
            dev(tim)->TAMATCHR = (timer_config[tim].cfg == GPT_CFG_32T) ?
                                 value : (LOAD_VALUE - value);
            dev(tim)->IMR |= GPT_IMR_TAMIM;
            break;
        case 1:
            dev(tim)->ICLR = GPT_IMR_TBMIM;
            dev(tim)->TBMATCHR = (timer_config[tim].cfg == GPT_CFG_32T) ?
                                 value : (LOAD_VALUE - value);
            dev(tim)->IMR |= GPT_IMR_TBMIM;
            break;
    }
    return 1;
}

int timer_clear(tim_t tim, int channel)
{
    if ((tim >= TIMER_NUMOF) || (channel >= timer_config[tim].chn)) {
        return -1;
    }

    dev(tim)->IMR &= (channel == 0) ? ~(GPT_IMR_TAMIM) : ~(GPT_IMR_TBMIM);

    return 0;
}

unsigned int timer_read(tim_t tim)
{
    if (tim >= TIMER_NUMOF) {
        return 0;
    }
    if (timer_config[tim].cfg == GPT_CFG_32T) {
        return dev(tim)->TAV;
    }
    return LOAD_VALUE - (dev(tim)->TAV & 0xFFFF);
}

void timer_stop(tim_t tim)
{
    if (tim < TIMER_NUMOF) {
        dev(tim)->CTL = 0;
    }
}

void timer_start(tim_t tim)
{
    if (tim < TIMER_NUMOF) {
        switch (timer_config[tim].chn) {
            case 1:
                dev(tim)->CTL = GPT_CTL_TAEN;
                break;
            case 2:
                dev(tim)->CTL = GPT_CTL_TAEN | GPT_CTL_TBEN;
                break;
        }
    }
}

/**
 * @brief   handle interrupt for a timer channel 0 (A)
 *
 * @param[in] tim   index of the timer
 */
static void irq_handler_a(tim_t tim) {
    uint32_t mis;
    /* Latch the active interrupt flags */
    mis = dev(tim)->MIS & TIMER_A_IRQ_MASK;
    /* Clear the latched interrupt flags */
    dev(tim)->ICLR = mis;

    if (mis & GPT_IMR_TAMIM) {
        /* Disable further match interrupts for this timer/channel */
        dev(tim)->IMR &= ~GPT_IMR_TAMIM;
        /* Invoke the callback function */
        ctx[tim].cb(ctx[tim].arg, 0);
    }

    cortexm_isr_end();
}

/**
 * @brief   handle interrupt for a timer channel 1 (B)
 *
 * @param[in] tim   index of the timer
 */
static void irq_handler_b(tim_t tim) {
    uint32_t mis;
    /* Latch the active interrupt flags */
    mis = dev(tim)->MIS & TIMER_B_IRQ_MASK;
    /* Clear the latched interrupt flags */
    dev(tim)->ICLR = mis;

    if (mis & GPT_IMR_TBMIM) {
        /* Disable further match interrupts for this timer/channel */
        dev(tim)->IMR &= ~GPT_IMR_TBMIM;
        /* Invoke the callback function */
        ctx[tim].cb(ctx[tim].arg, 1);
    }

    cortexm_isr_end();
}

void isr_timer0_chan0(void) {irq_handler_a(0);}
void isr_timer0_chan1(void) {irq_handler_b(0);}
void isr_timer1_chan0(void) {irq_handler_a(1);}
void isr_timer1_chan1(void) {irq_handler_b(1);}
void isr_timer2_chan0(void) {irq_handler_a(2);}
void isr_timer2_chan1(void) {irq_handler_b(2);}
void isr_timer3_chan0(void) {irq_handler_a(3);}
void isr_timer3_chan1(void) {irq_handler_b(3);}
