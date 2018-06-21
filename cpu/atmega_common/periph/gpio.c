/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA
 *               2018 Acutam Automation, LLC
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ATmega family
 *
 * @author      René Herthel <rene-herthel@outlook.de>
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp.h"
#endif

#define GPIO_BASE_PORT_A        (0x20)
#define GPIO_OFFSET_PORT_H      (0xCB)
#define GPIO_OFFSET_PIN_PORT    (0x02)
#define GPIO_OFFSET_PIN_PIN     (0x03)

/*
 * @brief     Define GPIO interruptions for an specific atmega CPU, by default
 *            2 (for small atmega CPUs)
 */
#if defined(INT7_vect)
#define GPIO_EXT_INT_NUMOF      (8U)
#elif defined(INT6_vect)
#define GPIO_EXT_INT_NUMOF      (7U)
#elif defined(INT5_vect)
#define GPIO_EXT_INT_NUMOF      (6U)
#elif defined(INT4_vect)
#define GPIO_EXT_INT_NUMOF      (5U)
#elif defined(INT3_vect)
#define GPIO_EXT_INT_NUMOF      (4U)
#elif defined(INT2_vect)
#define GPIO_EXT_INT_NUMOF      (3U)
#else
#define GPIO_EXT_INT_NUMOF      (2U)
#endif

static gpio_isr_ctx_t config[GPIO_EXT_INT_NUMOF];

/**
 * @brief     Extract the pin number of the given pin
 */
static inline uint8_t _pin_num(gpio_t pin)
{
    return (pin & 0x0f);
}

/**
 * @brief     Extract the port number of the given pin
 */
static inline uint8_t _port_num(gpio_t pin)
{
    return (pin >> 4) & 0x0f;
}

/**
 * @brief     Generate the PORTx address of the give pin.
 */
static inline uint16_t _port_addr(gpio_t pin)
{
    uint8_t port_num = _port_num(pin);
    uint16_t port_addr = port_num * GPIO_OFFSET_PIN_PIN;

    port_addr += GPIO_BASE_PORT_A;
    port_addr += GPIO_OFFSET_PIN_PORT;

#if defined (PORTG)
    if (port_num > PORT_G) {
        port_addr += GPIO_OFFSET_PORT_H;
    }
#endif
    return port_addr;
}

/**
 * @brief     Generate the DDRx address of the given pin
 */
static inline uint16_t _ddr_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x01);
}

/**
 * @brief     Generate the PINx address of the given pin.
 */
static inline uint16_t _pin_addr(gpio_t pin)
{
    return (_port_addr(pin) - 0x02);
}

static inline int8_t _int_num(gpio_t pin)
{
    uint8_t num;
    const gpio_t ext_ints[GPIO_EXT_INT_NUMOF] = CPU_ATMEGA_EXT_INTS;

    /* find pin in ext_ints array to get the interrupt number */
    for (num = 0; num < GPIO_EXT_INT_NUMOF; num++) {
        if (pin == ext_ints[num]) {
            return num;
        }
    }

    return -1;
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
        case GPIO_OUT:
            _SFR_MEM8(_ddr_addr(pin)) |= (1 << _pin_num(pin));
            break;
        case GPIO_IN:
            _SFR_MEM8(_ddr_addr(pin)) &= ~(1 << _pin_num(pin));
            _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
            break;
        case GPIO_IN_PU:
            _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
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

    int8_t int_num = _int_num(pin);

    if ((mode != GPIO_IN) && (mode != GPIO_IN_PU)) {
        return -1;
    }

    /* not a valid interrupt pin */
    if (int_num < 0) {
        return -1;
    }

    gpio_init(pin, mode);

    /* clear global interrupt flag */
    cli();

    /* enable interrupt number int_num */
    EIMSK |= (1 << int_num);

    /* configure the flank */
    if (flank > GPIO_RISING) {
        return -1;
    }

    /* apply flank to interrupt number int_num */
    if (int_num < 4) {
        EICRA |= (flank << (int_num * 2));
    }
#if defined(EICRB)
    else {
        EICRB |= (flank << ((int_num % 4) * 2));
    }
#endif

    /* set callback */
    config[int_num].cb = cb;
    config[int_num].arg = arg;

    /* set global interrupt flag */
    sei();

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

    EIMSK |= (1 << _int_num(pin));
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

    EIMSK &= ~(1 << _int_num(pin));
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

    return (_SFR_MEM8(_pin_addr(pin)) & (1 << _pin_num(pin)));
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
    if (gpio_read(pin)) {
        gpio_write(pin, 0);
    }
    else {
        gpio_write(pin, 1);
    }
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
        _SFR_MEM8(_port_addr(pin)) |= (1 << _pin_num(pin));
    }
    else {
        _SFR_MEM8(_port_addr(pin)) &= ~(1 << _pin_num(pin));
    }
}

static inline void irq_handler(uint8_t int_num)
{
    __enter_isr();
    config[int_num].cb(config[int_num].arg);
    __exit_isr();
}

ISR(INT0_vect, ISR_BLOCK)
{
    irq_handler(0); /**< predefined interrupt pin */
}

ISR(INT1_vect, ISR_BLOCK)
{
    irq_handler(1); /**< predefined interrupt pin */
}

#if defined(INT2_vect)
ISR(INT2_vect, ISR_BLOCK)
{
    irq_handler(2); /**< predefined interrupt pin */
}
#endif

#if defined(INT3_vect)
ISR(INT3_vect, ISR_BLOCK)
{
    irq_handler(3); /**< predefined interrupt pin */
}
#endif

#if defined(INT4_vect)
ISR(INT4_vect, ISR_BLOCK)
{
    irq_handler(4); /**< predefined interrupt pin */
}
#endif

#if defined(INT5_vect)
ISR(INT5_vect, ISR_BLOCK)
{
    irq_handler(5); /**< predefined interrupt pin */
}
#endif

#if defined(INT6_vect)
ISR(INT6_vect, ISR_BLOCK)
{
    irq_handler(6); /**< predefined interrupt pin */
}
#endif

#if defined(INT7_vect)
ISR(INT7_vect, ISR_BLOCK)
{
    irq_handler(7); /**< predefined interrupt pin */
}
#endif
