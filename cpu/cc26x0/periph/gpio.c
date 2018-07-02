/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc26x0
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Leon M. George <leon@georgemail.eu>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp.h"
#endif

#define GPIO_ISR_CHAN_NUMOF     (32)

#define DOE_SHIFT               (29U)

/**
 * @brief   static callback memory
 */
static gpio_isr_ctx_t gpio_chan[GPIO_ISR_CHAN_NUMOF];

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

    if ((unsigned int)pin > 31)
        return -1;

    /* enable GPIO clock */
    PRCM->PDCTL0 |= PDCTL0_PERIPH_ON;
    while(!(PRCM->PDSTAT0 & PDSTAT0_PERIPH_ON)) ;
    PRCM->GPIOCLKGR |= 1;
    PRCM->CLKLOADCTL |= CLKLOADCTL_LOAD;
    while (!(PRCM->CLKLOADCTL & CLKLOADCTL_LOADDONE)) ;

    /* configure the GPIO mode */
    IOC->CFG[pin] = mode;
    GPIO->DOE &= ~(1 << pin);
    GPIO->DOE |= ((~(mode >> DOE_SHIFT) & 0x1) << pin);

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

    int init = gpio_init(pin, mode);
    if (init != 0)
        return init;

    NVIC_EnableIRQ(EDGE_DETECT_IRQN);

    IOC->CFG[pin] |= flank;

    gpio_chan[pin].cb = cb;
    gpio_chan[pin].arg = arg;

    /* clears the interrupt flag */
    GPIO->EVFLAGS |= (1 << pin);

    gpio_irq_enable(pin);

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

    IOC->CFG[pin] |= IOCFG_EDGEIRQ_ENABLE;
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

    IOC->CFG[pin] &= ~IOCFG_EDGEIRQ_ENABLE;
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

    if (GPIO->DOE & (1 << pin)) {
        return (GPIO->DOUT >> pin) & 1;
    }
    else {
        return (GPIO->DIN >> pin) & 1;
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

    GPIO->DOUTTGL = (1 << pin);
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
        GPIO->DOUTSET = (1 << pin);
    } else {
        GPIO->DOUTCLR = (1 << pin);
    }
}

void isr_edge(void)
{
    for (unsigned pin = 0; pin < GPIO_ISR_CHAN_NUMOF; pin++) {
        /* doc claims EVFLAGS will only be set for pins that have edge detection enabled */
        if (GPIO->EVFLAGS & (1 << pin)) {
            GPIO->EVFLAGS |= (1 << pin);
            gpio_chan[pin].cb(gpio_chan[pin].arg);
        }
    }
    cortexm_isr_end();
}
