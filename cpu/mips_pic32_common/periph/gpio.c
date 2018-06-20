/*
 * Copyright(C) 2017 Imagination Technologies Limited and/or its
 *              affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

 /**
  * @ingroup     cpu_mips_pic32_common
  * @ingroup     drivers_periph_gpio
  * @{
  *
  * @file
  * @brief       Low-level GPIO driver implementation
  *
  * @}
  */

#include <assert.h>
#include <stdint.h>
#include "periph/gpio.h"
#include "board.h"

#ifdef MODULE_GPIO_EXP
#include "gpio_exp.h"
#endif

#define GPIO_PIN_NO(PIN)    (1 << ((PIN) & 0xf))
#define GPIO_PORT(PIN)      ((PIN) >> 4)

#define LATx(P)         (base_address[P].gpio[0x10/0x4])
#define LATxCLR(P)      (base_address[P].gpio[0x14/0x4])
#define LATxSET(P)      (base_address[P].gpio[0x18/0x4])
#define LATxINV(P)      (base_address[P].gpio[0x1C/0x4])
#define PORTx(P)        (base_address[P].gpio[0x0])
#define CNPUxCLR(P)     (base_address[P].gpio[0x34/0x4])
#define CNPUxSET(P)     (base_address[P].gpio[0x38/0x4])
#define CNPDxCLR(P)     (base_address[P].gpio[0x44/0x4])
#define CNPDxSET(P)     (base_address[P].gpio[0x48/0x4])
#define ODCxCLR(P)      (base_address[P].gpio[0x24/0x4])
#define ODCxSET(P)      (base_address[P].gpio[0x28/0x4])
#define ANSELxCLR(P)    (base_address[P].ansel[0x04/0x4])
#define TRISxCLR(P)     (base_address[P].tris[0x04/0x4])
#define TRISxSET(P)     (base_address[P].tris[0x08/0x4])

typedef struct PIC32_GPIO_tag {
    volatile uint32_t* gpio;
    volatile uint32_t* ansel;
    volatile uint32_t* tris;
} PIC32_GPIO_T;

static PIC32_GPIO_T base_address[] = {
#ifdef _PORTA_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTA_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELA,
        .tris  = (volatile uint32_t*)&TRISA
    },
#else
    {0 , 0, 0},
#endif
#ifdef _PORTB_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTB_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELB,
        .tris  = (volatile uint32_t*)&TRISB
    },
#else
    {0 , 0, 0},
#endif
#ifdef _PORTC_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTC_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELC,
        .tris  = (volatile uint32_t*)&TRISC
    },
#else
    {0 , 0, 0},
#endif
#ifdef _PORTD_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTD_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELD,
        .tris  = (volatile uint32_t*)&TRISD
    },
#else
    {0 , 0, 0},
#endif
#ifdef _PORTE_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTE_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELE,
        .tris  = (volatile uint32_t*)&TRISE
    },
#else
    {0 , 0, 0},
#endif
#ifdef _PORTF_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTF_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELF,
        .tris  = (volatile uint32_t*)&TRISF
    },
#else
    {0 , 0, 0},
#endif
#ifdef _PORTG_BASE_ADDRESS
    {
        .gpio  = (volatile uint32_t*)_PORTG_BASE_ADDRESS,
        .ansel = (volatile uint32_t*)&ANSELG,
        .tris  = (volatile uint32_t*)&TRISG
    },
#else
    {0 , 0, 0},
#endif
};

static inline int check_valid_port(uint8_t port)
{
    return port < (sizeof(base_address)/sizeof(base_address[0]))
        && base_address[port].gpio != NULL;
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

    uint8_t port = GPIO_PORT(pin);
    uint32_t pin_no = GPIO_PIN_NO(pin);
    uint8_t output = 0, pu = 0, pd = 0, od = 0;

    assert(check_valid_port(port));

    switch (mode) {
        case GPIO_IN:
            break;
        case GPIO_IN_PD:
            pd = 1;
            break;
        case GPIO_IN_PU:
            pu = 1;
            break;

        case GPIO_OD_PU:
            pu = 1;
        case GPIO_OD:
            od = 1;
        case GPIO_OUT:
            output = 1;
            break;
    }

    ANSELxCLR(port) = pin_no;       /* Configure GPIO as digital */

    if (pu)
        CNPUxSET(port) = pin_no;
    else
        CNPUxCLR(port) = pin_no;

    if (pd)
        CNPDxSET(port) = pin_no;
    else
        CNPDxCLR(port) = pin_no;

    if (od)
        ODCxSET(port) = pin_no;
    else
        ODCxCLR(port) = pin_no;

    if (output)
        TRISxCLR(port) = pin_no;
    else
        TRISxSET(port) = pin_no;

    return 0;
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
/* NOTE: gpio_exp ints not expected to work until normal ints implemented */
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

    (void)pin;
    (void)mode;
    (void)flank;
    (void)cb;
    (void)arg;

    /* TODO: Not implemented yet */

    return -1;
}

void gpio_irq_enable(gpio_t pin)
{
/* NOTE: gpio_exp irqs not expected to work until normal irqs implemented */
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

    (void)pin;

    /* TODO: Not implemented yet */
}

void gpio_irq_disable(gpio_t pin)
{
/* NOTE: gpio_exp irqs not expected to work until normal irqs implemented */
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

    (void)pin;

    /* TODO: Not implemented yet */
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

    assert(check_valid_port(GPIO_PORT(pin)));

    return PORTx(GPIO_PORT(pin)) & GPIO_PIN_NO(pin);
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

    assert(check_valid_port(GPIO_PORT(pin)));

    LATxINV(GPIO_PORT(pin)) = GPIO_PIN_NO(pin);
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

    assert(check_valid_port(GPIO_PORT(pin)));

    if (value) {
        LATxSET(GPIO_PORT(pin)) = GPIO_PIN_NO(pin);
    }
    else {
        LATxCLR(GPIO_PORT(pin)) = GPIO_PIN_NO(pin);
    }
}
