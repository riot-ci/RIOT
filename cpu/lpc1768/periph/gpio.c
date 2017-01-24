/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lpc1768
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph/gpio.h"

#define PIN_MASK        (0x00FF)
#define PORT_SHIFT      (8U)

#define ISR_NUMOF       (4U)

typedef struct {
    __IO uint32_t PINSEL[11];
    uint32_t RESERVED0[5];
    __IO uint32_t PINMODE[10];
    __IO uint32_t PINMODE_OD[5];
    __IO uint32_t I2CPADCFG;
    __IO uint32_t p[5][32];
} LPC_IOCON_Typedef;

#define LPC_IOCON ((LPC_IOCON_Typedef *) LPC_PINCON_BASE)

static gpio_t isrmap[] = { GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF, GPIO_UNDEF };

static gpio_isr_ctx_t isrctx[ISR_NUMOF];

static inline int _pin(gpio_t pin)
{
    return (pin & PIN_MASK);
}

static inline int _port(gpio_t pin)
{
    return (pin >> PORT_SHIFT);
}

static inline LPC_GPIO_TypeDef *_base(gpio_t pin)
{
    return (LPC_GPIO_TypeDef *) (LPC_GPIO_BASE + (_port(pin) * 0x20));
}

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    if (_port(pin) > 4) {
        return -1;
    }

    if (_pin(pin) > 32) {
        return -1;
    }

    /* enable gpio peripheral */
    LPC_SC->PCONP |= (1 << 15);

    /* pin as output or input */
    LPC_GPIO_TypeDef *base = _base(pin);

    if (mode & 0x1) {
        base->FIODIR |= (1 << _pin(pin));
    }
    else {
        base->FIODIR &= ~(1 << _pin(pin));
    }

    int reg = 2 * _port(pin) + (_pin(pin) / 16);
    int bit = (pin % 16) * 2;

    /* pin function */
    LPC_IOCON->PINSEL[reg] &= ~(0x3 << bit);

    /* pull up or pull down */
    if (mode & 0x2) {
        LPC_IOCON->PINMODE[reg] &= (0x3 << bit);
    }
    else {
        LPC_IOCON->PINMODE[reg] &= (0x3 << bit);
        LPC_IOCON->PINMODE[reg] |= (0x1 << bit);
    }

    /* open drain */
    if (mode & 0x4) {
        LPC_IOCON->PINMODE_OD[_port(pin)] |= (1 << _pin(pin));
    }

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* make sure we have an interrupt channel available */
    int i = 0;

    while ((i < ISR_NUMOF) && (isrmap[i] != GPIO_UNDEF) && (isrmap[i] != pin)) {
        i++;
    }
    if (i == ISR_NUMOF) {
        return -1;
    }

    /* do basic pin configuration */
    if (gpio_init(pin, mode) != 0) {
        return -1;
    }

    /* enable power for GPIO pin interrupt interface */


    /* save ISR context */
    isrctx[i].cb = cb;
    isrctx[i].arg = arg;
    isrmap[i] = pin;

    /* set active flank configuration */
    switch (flank) {
        case GPIO_RISING:

            break;
        case GPIO_FALLING:

            break;
        case GPIO_BOTH:
            break;
        default:
            return -1;
    }

    /* clear any pending requests and enable the interrupt */
    LPC_SC->EXTINT = (1 << i);
    NVIC_EnableIRQ(EINT0_IRQn + i);

    return 0;
}

void gpio_irq_enable(gpio_t pin)
{
    for (int i = 0; i < ISR_NUMOF; i++) {
        if (isrmap[i] == pin) {
            NVIC_EnableIRQ(EINT0_IRQn + i);
        }
    }
}

void gpio_irq_disable(gpio_t pin)
{
    for (int i = 0; i < ISR_NUMOF; i++) {
        if (isrmap[i] == pin) {
            NVIC_DisableIRQ(EINT0_IRQn + i);
        }
    }
}

int gpio_read(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    return base->FIOPIN & (1 << _pin(pin)) ? 1 : 0;
}

void gpio_set(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    base->FIOSET |= (1 << _pin(pin));
}

void gpio_clear(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    base->FIOCLR |= (1 << _pin(pin));
}

void gpio_toggle(gpio_t pin)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    if (base->FIOSET & _pin(pin)) {
        base->FIOCLR |= (1 << _pin(pin));
    }
    else {
        base->FIOSET |= (1 << _pin(pin));
    }
}

void gpio_write(gpio_t pin, int value)
{
    LPC_GPIO_TypeDef *base = _base(pin);

    if (value) {
        base->FIOSET |= (1 << _pin(pin));
    }
    else {
        base->FIOCLR |= (1 << _pin(pin));
    }
}

static inline void isr_common(uint8_t channel)
{
    LPC_SC->EXTINT = (1 << channel);
    isrctx[channel].cb(isrctx[channel].arg);

    cortexm_isr_end();
}

void isr_eint0(void)
{
    isr_common(0);
}
void isr_eint1(void)
{
    isr_common(1);
}
void isr_eint2(void)
{
    isr_common(2);
}
void isr_eint3(void)
{
    isr_common(3);
}
