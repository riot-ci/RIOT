/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_nrf51
 * @ingroup     drivers_periph_pwm
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
*
 * @note        This driver initializes a duty cycle of '1'
 *
 * @author      Semjon Kerner <semjon.kerner@fu-berlin.de>
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <math.h>

#include "periph/gpio.h"
#include "periph/pwm.h"
#include "vectors_cortexm.h"

#define ENABLE_DEBUG                (1)
#include "debug.h"
#define DEBUG_ASSERT_VERBOSE        (1U)

#define NRF51_PWM_PRESCALER_MIN     (0U)
#define NRF51_PWM_PRESCALER_MAX     (10U)
#define NRF51_PWM_PPI_CHANNELS      (((1U) << PWM_PPI_A) | ((1U) << PWM_PPI_B))
#ifndef NRF51_PWM_PERCENT_VAL
    #define NRF51_PWM_PERCENT_VAL   (1U)
#endif

inline uint32_t apply_prescaler(uint8_t prescaler)
{
    return 16000000 >> prescaler;
}

/*
 * compare requested frequency with available frequency
 * has a margin of PERCENT_VAL
 */
bool is_close_to(uint32_t f_requ, uint32_t f_avail)
{
    int percent = (f_avail / 100) * NRF51_PWM_PERCENT_VAL;
    if (f_requ > (f_avail - percent) && f_requ < (f_avail + percent)) {
        return true;
    }
    return false;
}

uint32_t pwm_init(pwm_t dev, pwm_mode_t mode, uint32_t freq, uint16_t res)
{
    assert (dev == 0 && ((mode == PWM_LEFT) || (mode == PWM_RIGHT)));

    /* reset and configure the timer */
    PWM_TIMER->TASKS_STOP = 1;
    PWM_TIMER->BITMODE = TIMER_BITMODE_BITMODE_32Bit;
    PWM_TIMER->MODE = TIMER_MODE_MODE_Timer;
    PWM_TIMER->TASKS_CLEAR = 1;

    /* calculate and set prescaler */
    uint32_t timer_freq = freq * res;
    for (uint32_t ps = NRF51_PWM_PRESCALER_MIN;
         ps <= NRF51_PWM_PRESCALER_MAX;
         ps++) {
        if (ps == NRF51_PWM_PRESCALER_MAX) {
            DEBUG("[pwm] init error: resolution or frequency not supported\n");
            return 0;
        }
        if (is_close_to(timer_freq, apply_prescaler(ps))){
            PWM_TIMER->PRESCALER = (uint32_t)ps;
            break;
        }
    }

    /* reset timer compare events */
    PWM_TIMER->EVENTS_COMPARE[0] = 0;
    PWM_TIMER->EVENTS_COMPARE[1] = 0;
    /* init timer compare values */
    PWM_TIMER->CC[0] = 1;
    PWM_TIMER->CC[1] = res;

    /* configure PPI Event (set compare values and pwm width) */
    if (mode == PWM_LEFT) {
        NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] = GPIOTE_CONFIG_MODE_Task     |
                                            (PWM_PIN << 8)              |
                                            GPIOTE_CONFIG_POLARITY_Msk  |
                                            GPIOTE_CONFIG_OUTINIT_Msk;
    }
    else if (mode == PWM_RIGHT) {
        NRF_GPIOTE->CONFIG[PWM_GPIOTE_CH] = GPIOTE_CONFIG_MODE_Task     |
                                            (PWM_PIN << 8)              |
                                            GPIOTE_CONFIG_POLARITY_Msk;
    }

    /* configure PPI Channels (connect compare-event and gpiote-task) */
    NRF_PPI->CH[PWM_PPI_A].EEP = (uint32_t)(&PWM_TIMER->EVENTS_COMPARE[0]);
    NRF_PPI->CH[PWM_PPI_B].EEP = (uint32_t)(&PWM_TIMER->EVENTS_COMPARE[1]);

    NRF_PPI->CH[PWM_PPI_A].TEP =
        (uint32_t)(&NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH]);
    NRF_PPI->CH[PWM_PPI_B].TEP =
        (uint32_t)(&NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH]);

    /* enable configured PPI Channels */
    NRF_PPI->CHENSET = NRF51_PWM_PPI_CHANNELS;

    /* shortcut to reset Counter after CC[1] event */
    PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Msk;

    PWM_TIMER->TASKS_START = 1;

    DEBUG("Timer frequency is set to %ld\n",
          apply_prescaler(PWM_TIMER->PRESCALER));

    return (uint32_t)(apply_prescaler(PWM_TIMER->PRESCALER) / res);
}

void pwm_set(pwm_t dev, uint8_t channel, uint16_t value)
{
    assert((dev == 0) && (channel == 0));

    /*
     * make sure duty cycle is set at the beggining of each period
     * ensure to stop the timer as soon as possible
     */
    PWM_TIMER->TASKS_STOP = 1;
    PWM_TIMER->EVENTS_COMPARE[1] = 0;
    PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE1_STOP_Msk;
    PWM_TIMER->TASKS_START = 1;

    /*
     * waiting for the timer to stop
     * This loop generates heavy load. This is not optimal therefore a local
     * sleep function should be implemented.
     */
    while (PWM_TIMER->EVENTS_COMPARE[1] == 0) {};

    /*
     * guarding the compare event
     * in case that the duty cycle is 0% (100%):
     * - disable ppi channels
     * - remember state in CC[0]
     * - trigger the GPIOTE to set it to '0' ('1')
     */
    if (value == 0) {
        if (PWM_TIMER->CC[0] != 0) {
            NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
        }

        NRF_PPI->CHENCLR = NRF51_PWM_PPI_CHANNELS;
        PWM_TIMER->CC[0] = 0;
    }
    else if (value >= PWM_TIMER->CC[1]) {
        if (PWM_TIMER->CC[0] == 0) {
            NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
        }

        NRF_PPI->CHENCLR = NRF51_PWM_PPI_CHANNELS;
        PWM_TIMER->CC[0] = PWM_TIMER->CC[1];
    }
    else {
        if (NRF_PPI->CHEN != NRF51_PWM_PPI_CHANNELS) {
            if (PWM_TIMER->CC[0] == 0) {
                NRF_GPIOTE->TASKS_OUT[PWM_GPIOTE_CH] = 1;
            }
            NRF_PPI->CHENSET = NRF51_PWM_PPI_CHANNELS;
        }

        PWM_TIMER->CC[0] = value;
    }
    /* reconfigure pwm to standard mode */
    PWM_TIMER->EVENTS_COMPARE[1] = 0;
    PWM_TIMER->TASKS_CLEAR = 1;
    PWM_TIMER->SHORTS = TIMER_SHORTS_COMPARE1_CLEAR_Msk;
    PWM_TIMER->TASKS_START = 1;
}

uint8_t pwm_channels(pwm_t dev)
{
    assert(dev == 0);
    return 1;
}

void pwm_poweron(pwm_t dev)
{
    assert(dev == 0);

#if CPU_FAM_NRF51
    PWM_TIMER->POWER = 1;
    PWM_TIMER->TASKS_START = 1;
#endif
}

void pwm_poweroff(pwm_t dev)
{
    assert(dev == 0);

#if CPU_FAM_NRF51
    PWM_TIMER->TASKS_STOP = 1;
    PWM_TIMER->POWER = 0;
#endif
}
