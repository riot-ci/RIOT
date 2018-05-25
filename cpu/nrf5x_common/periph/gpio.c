/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 *               2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_nrf5x_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @note        This GPIO driver implementation supports only one pin to be
 *              defined as external interrupt.
 *
 * @author      Christian Kühling <kuehling@zedat.fu-berlin.de>
 * @author      Timo Ziegler <timo.ziegler@fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Jan Wagner <mail@jwagner.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp.h"
#endif

#define PORT_BIT            (1 << 5)
#define PIN_MASK            (0x1f)

/**
 * @brief   Place to store the interrupt context
 */
static gpio_isr_ctx_t exti_chan;

/**
 * @brief   Get the port's base address
 */
static inline NRF_GPIO_Type* port(gpio_t pin)
{
#if (CPU_FAM_NRF51)
    (void) pin;
    return NRF_GPIO;
#elif defined(CPU_MODEL_NRF52832XXAA)
    (void) pin;
    return NRF_P0;
#else
    return (pin & PORT_BIT) ? NRF_P1 : NRF_P0;
#endif
}

/**
 * @brief   Get a pin's offset
 */
static inline int pin_num(gpio_t pin)
{
#ifdef CPU_MODEL_NRF52840XXAA
    return (pin & PIN_MASK);
#else
    return (int)pin;
#endif
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
#ifdef MODULE_GPIO_EXP
    /* Redirect pin handling to GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        gpio_exp_t *exp_entry = gpio_exp_entry(pin);

        if (exp_entry == NULL) {
            return -1;
        }

        return exp_entry->driver->init(exp_entry->dev, gpio_exp_pin(pin), mode);
    }
#endif /* MODULE_GPIO_EXP */

    switch (mode) {
        case GPIO_IN:
        case GPIO_IN_PD:
        case GPIO_IN_PU:
        case GPIO_OUT:
            /* configure pin direction, input buffer and pull resistor state */
            port(pin)->PIN_CNF[pin] = mode;
            break;
        default:
            return -1;
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
#ifdef MODULE_GPIO_EXP
    /* Redirect pin handling to GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        gpio_exp_t *exp_entry = gpio_exp_entry(pin);

        if (exp_entry == NULL) {
            return -1;
        }

        return exp_entry->driver->init_int(exp_entry->dev, gpio_exp_pin(pin),
                                           mode, flank, cb, arg);
    }
#endif /* MODULE_GPIO_EXP */

    /* disable external interrupt in case one is active */
    NRF_GPIOTE->INTENSET &= ~(GPIOTE_INTENSET_IN0_Msk);
    /* save callback */
    exti_chan.cb = cb;
    exti_chan.arg = arg;
    /* configure pin as input */
    gpio_init(pin, mode);
    /* set interrupt priority and enable global GPIOTE interrupt */
    NVIC_EnableIRQ(GPIOTE_IRQn);
    /* configure the GPIOTE channel: set even mode, pin and active flank */
    NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Event |
                             (pin << GPIOTE_CONFIG_PSEL_Pos) |
#ifdef CPU_MODEL_NRF52840XXAA
                             ((pin & PORT_BIT) << 8) |
#endif
                             (flank << GPIOTE_CONFIG_POLARITY_Pos));
    /* enable external interrupt */
    NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;
    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
#ifdef MODULE_GPIO_EXP
    /* Redirect pin handling to GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        gpio_exp_t *exp_entry = gpio_exp_entry(pin);

        if (exp_entry == NULL) {
            return;
        }

        exp_entry->driver->irq(exp_entry->dev, gpio_exp_pin(pin), 1);
        return;
    }
#endif /* MODULE_GPIO_EXP */

    (void) pin;
    NRF_GPIOTE->INTENSET |= GPIOTE_INTENSET_IN0_Msk;
}

void gpio_irq_disable(gpio_t pin)
{
#ifdef MODULE_GPIO_EXP
    /* Redirect pin handling to GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        gpio_exp_t *exp_entry = gpio_exp_entry(pin);

        if (exp_entry == NULL) {
            return;
        }

        exp_entry->driver->irq(exp_entry->dev, gpio_exp_pin(pin), 0);
        return;
    }
#endif /* MODULE_GPIO_EXP */

    (void) pin;
    NRF_GPIOTE->INTENCLR |= GPIOTE_INTENSET_IN0_Msk;
}

int gpio_read(gpio_t pin)
{
#ifdef MODULE_GPIO_EXP
    /* Redirect pin handling to GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        gpio_exp_t *exp_entry = gpio_exp_entry(pin);

        if (exp_entry == NULL) {
            return -1;
        }

        return exp_entry->driver->read(exp_entry->dev, gpio_exp_pin(pin));
    }
#endif /* MODULE_GPIO_EXP */

    if (port(pin)->DIR & (1 << pin)) {
        return (port(pin)->OUT & (1 << pin)) ? 1 : 0;
    }
    else {
        return (port(pin)->IN & (1 << pin)) ? 1 : 0;
    }
}

void gpio_set(gpio_t pin)
{
    gpio_write(pin, 1);
}

void gpio_clear(gpio_t pin)
{
    gpio_write(pin, 0);
}

void gpio_toggle(gpio_t pin)
{
#ifdef MODULE_GPIO_EXP
    /* Read then write if pin is on GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        if (gpio_read(pin)) {
            gpio_write(pin, 0);
        }
        else {
            gpio_write(pin, 1);
        }

        return;
    }
#endif /* MODULE_GPIO_EXP */

    port(pin)->OUT ^= (1 << pin);
}

void gpio_write(gpio_t pin, int value)
{
#ifdef MODULE_GPIO_EXP
    /* Redirect pin handling to GPIO expander */
    if (pin > GPIO_EXP_THRESH) {
        gpio_exp_t *exp_entry = gpio_exp_entry(pin);

        if (exp_entry == NULL) {
            return;
        }

        exp_entry->driver->write(exp_entry->dev, gpio_exp_pin(pin), value);
        return;
    }
#endif /* MODULE_GPIO_EXP */

    if (value) {
        port(pin)->OUTSET = (1 << pin);
    } else {
        port(pin)->OUTCLR = (1 << pin);
    }
}

void isr_gpiote(void)
{
    if (NRF_GPIOTE->EVENTS_IN[0] == 1) {
        NRF_GPIOTE->EVENTS_IN[0] = 0;
        exti_chan.cb(exti_chan.arg);
    }
    cortexm_isr_end();
}
