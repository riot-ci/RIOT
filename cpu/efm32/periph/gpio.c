/*
 * Copyright (C) 2015-2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Hauke Petersen <mail@haukepetersen.de>
 * @author      Ryan Kurte <ryankurte@gmail.com>
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"

#include "periph/gpio.h"

#include "em_gpio.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp.h"
#endif

/**
 * @brief   Number of external interrupt lines.
 */
#define NUMOF_IRQS         (GPIO_PIN_MAX)

/**
 * @brief   Hold one interrupt context per interrupt line
 */
static gpio_isr_ctx_t isr_ctx[NUMOF_IRQS];

static inline GPIO_Port_TypeDef _port_num(gpio_t pin)
{
    return ((pin & 0xf0) >> 4);
}

static inline uint32_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

static inline uint32_t _pin_mask(gpio_t pin)
{
    return (1 << _pin_num(pin));
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

    /* check for valid pin */
    if (pin == GPIO_UNDEF) {
        return -1;
    }

    /* enable clocks */
    CMU_ClockEnable(cmuClock_HFPER, true);
    CMU_ClockEnable(cmuClock_GPIO, true);

    /* configure pin */
    GPIO_PinModeSet(_port_num(pin), _pin_num(pin), mode >> 1, mode & 0x1);
#ifdef _SILICON_LABS_32B_SERIES_0
    GPIO_DriveModeSet(_port_num(pin), gpioDriveModeStandard);
#endif

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

    int result = gpio_init(pin, mode);

    if (result != 0) {
        return result;
    }

    /* just in case, disable the interrupt for this pin */
    GPIO_IntDisable(_pin_num(pin));

    /* store interrupt callback */
    isr_ctx[_pin_num(pin)].cb = cb;
    isr_ctx[_pin_num(pin)].arg = arg;

    /* enable interrupts */
    GPIO_IntConfig(_port_num(pin), _pin_num(pin),
                   flank & GPIO_RISING, flank & GPIO_FALLING, true);

    NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
    NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);

    NVIC_EnableIRQ(GPIO_EVEN_IRQn);
    NVIC_EnableIRQ(GPIO_ODD_IRQn);

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

    GPIO_IntEnable(_pin_mask(pin));
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

    GPIO_IntDisable(_pin_mask(pin));
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

    return GPIO_PinInGet(_port_num(pin), _pin_num(pin));
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

    GPIO_PinOutToggle(_port_num(pin), _pin_num(pin));
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
        GPIO_PinOutSet(_port_num(pin), _pin_num(pin));
    }
    else {
        GPIO_PinOutClear(_port_num(pin), _pin_num(pin));
    }
}

/**
 * @brief   Actual interrupt handler for both even and odd pin index numbers.
 */
static void gpio_irq(void)
{
    for (int i = 0; i < NUMOF_IRQS; i++) {
        if (GPIO_IntGet() & (1 << i)) {
            isr_ctx[i].cb(isr_ctx[i].arg);
            GPIO_IntClear(1 << i);
        }
    }
    cortexm_isr_end();
}

/**
 * @brief   External interrupt handler for even pin index numbers
 */
void isr_gpio_even(void)
{
    gpio_irq();
}

/**
 * @brief   External interrupt handler for odd pin index numbers
 */
void isr_gpio_odd(void)
{
    gpio_irq();
}
