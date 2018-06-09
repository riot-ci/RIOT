/*
 * Copyright 2017 Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_fe310
 * @{
 *
 * @file        gpio.c
 * @brief       Low-level GPIO implementation
 *
 * @author      Ken Rabold
 * @}
 */

#include <stdlib.h>
#include <unistd.h>

#include "irq.h"
#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "vendor/encoding.h"
#include "vendor/platform.h"
#include "vendor/plic_driver.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp.h"
#endif

/* Num of GPIOs supported */
#define GPIO_NUMOF (32)

static gpio_flank_t isr_flank[GPIO_NUMOF];
static gpio_isr_ctx_t isr_ctx[GPIO_NUMOF];

void gpio_isr(int num)
{
    uint32_t pin = num - INT_GPIO_BASE;

    /* Invoke callback function */
    if (isr_ctx[pin].cb) {
        isr_ctx[pin].cb(isr_ctx[pin].arg);
    }

    /* Clear interupt */
    switch (isr_flank[pin]) {
        case GPIO_FALLING:
            GPIO_REG(GPIO_FALL_IP) |= (1 << pin);
            break;

        case GPIO_RISING:
            GPIO_REG(GPIO_RISE_IP) |= (1 << pin);
            break;

        case GPIO_BOTH:
            GPIO_REG(GPIO_FALL_IP) |= (1 << pin);
            GPIO_REG(GPIO_RISE_IP) |= (1 << pin);
            break;
    }
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

    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return -1;
    }

    /*  Configure the mode */
    switch (mode) {
        case GPIO_IN:
            GPIO_REG(GPIO_INPUT_EN) |= (1 << pin);
            GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << pin);
            GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << pin);
            break;

        case GPIO_IN_PU:
            GPIO_REG(GPIO_INPUT_EN) |= (1 << pin);
            GPIO_REG(GPIO_OUTPUT_EN) &= ~(1 << pin);
            GPIO_REG(GPIO_PULLUP_EN) |= (1 << pin);
            break;

        case GPIO_OUT:
            GPIO_REG(GPIO_INPUT_EN) &= ~(1 << pin);
            GPIO_REG(GPIO_OUTPUT_EN) |= (1 << pin);
            GPIO_REG(GPIO_PULLUP_EN) &= ~(1 << pin);
            break;

        default:
            return -1;
    }

    /* Configure the pin muxing for the GPIO */
    GPIO_REG(GPIO_IOF_EN) &= ~(1 << pin);
    GPIO_REG(GPIO_IOF_SEL) &= ~(1 << pin);

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

    /* Configure pin */
    if (gpio_init(pin, mode) != 0) {
        return -1;
    }

    /* Disable ext interrupts when setting up */
    clear_csr(mie, MIP_MEIP);

    /* Configure GPIO ISR with PLIC */
    set_external_isr_cb(INT_GPIO_BASE + pin, gpio_isr);
    PLIC_enable_interrupt(INT_GPIO_BASE + pin);
    PLIC_set_priority(INT_GPIO_BASE + pin, GPIO_INTR_PRIORITY);

    /*  Configure the active flank(s) */
    gpio_irq_enable(pin);

    /* Save callback */
    isr_ctx[pin].cb = cb;
    isr_ctx[pin].arg = arg;
    isr_flank[pin] = flank;

    /* Re-eanble ext interrupts */
    set_csr(mie, MIP_MEIP);

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

    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return;
    }

    /* Enable interupt for pin */
    switch (isr_flank[pin]) {
        case GPIO_FALLING:
            GPIO_REG(GPIO_FALL_IE) |= (1 << pin);
            break;

        case GPIO_RISING:
            GPIO_REG(GPIO_RISE_IE) |= (1 << pin);
            break;

        case GPIO_BOTH:
            GPIO_REG(GPIO_FALL_IE) |= (1 << pin);
            GPIO_REG(GPIO_RISE_IE) |= (1 << pin);
            break;

        default:
            break;
    }
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

    /* Check for valid pin */
    if (pin >= GPIO_NUMOF) {
        return;
    }

    /* Disable interupt for pin */
    switch (isr_flank[pin]) {
        case GPIO_FALLING:
            GPIO_REG(GPIO_FALL_IE) &= ~(1 << pin);
            break;

        case GPIO_RISING:
            GPIO_REG(GPIO_RISE_IE) &= ~(1 << pin);
            break;

        case GPIO_BOTH:
            GPIO_REG(GPIO_FALL_IE) &= ~(1 << pin);
            GPIO_REG(GPIO_RISE_IE) &= ~(1 << pin);
            break;

        default:
            break;
    }
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

    return (GPIO_REG(GPIO_INPUT_VAL) & (1 << pin)) ? 1 : 0;
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

    GPIO_REG(GPIO_OUTPUT_VAL) ^= (1 << pin);
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
        GPIO_REG(GPIO_OUTPUT_VAL) |= (1 << pin);
    }
    else {
        GPIO_REG(GPIO_OUTPUT_VAL) &= ~(1 << pin);
    }
}
