/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA

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
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 * @author      Torben Petersen <petersen@ibr.cs.tu-bs.de>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include <avr/interrupt.h>

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "atmega_gpio.h"

#define ENABLE_DEBUG            (0)
#include "debug.h"

#ifdef MODULE_PERIPH_GPIO_IRQ
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

/* Detects amount of possible PCINTs */
#if defined(MODULE_ATMEGA_PCINT0) || defined(MODULE_ATMEGA_PCINT1) || \
    defined(MODULE_ATMEGA_PCINT2) || defined(MODULE_ATMEGA_PCINT3)
#include "atmega_pcint.h"

#define ENABLE_PCINT

/* Check which pcints should be enabled */
#if defined(MODULE_ATMEGA_PCINT0) && !defined(ATMEGA_PCINT_MAP_PCINT0)
#error \
    Either mapping for pin change interrupt bank 0 is missing or not supported by the MCU
#endif

#if defined(MODULE_ATMEGA_PCINT1) && !defined(ATMEGA_PCINT_MAP_PCINT1)
#error \
    Either mapping for pin change interrupt bank 1 is missing or not supported by the MCU
#endif

#if defined(MODULE_ATMEGA_PCINT2) && !defined(ATMEGA_PCINT_MAP_PCINT2)
#error \
    Either mapping for pin change interrupt bank 2 is missing or not supported by the MCU
#endif

#if defined(MODULE_ATMEGA_PCINT3) && !defined(ATMEGA_PCINT_MAP_PCINT3)
#error \
    Either mapping for pin change interrupt bank 3 is missing or not supported by the MCU
#endif

/**
 * @brief   Use anonymous enum as for addressing the @ref pcint_state
 */
enum {
#ifdef MODULE_ATMEGA_PCINT0
    PCINT0_IDX,     /**< Index of PCINT0, if used */
#endif /* MODULE_ATMEGA_PCINT0 */
#ifdef MODULE_ATMEGA_PCINT1
    PCINT1_IDX,     /**< Index of PCINT1, if used */
#endif /* MODULE_ATMEGA_PCINT1 */
#ifdef MODULE_ATMEGA_PCINT2
    PCINT2_IDX,     /**< Index of PCINT2, if used */
#endif /* MODULE_ATMEGA_PCINT2 */
#ifdef MODULE_ATMEGA_PCINT3
    PCINT3_IDX,     /**< Index of PCINT3, if used */
#endif /* MODULE_ATMEGA_PCINT3 */
    PCINT_NUM_BANKS     /**< Number of PCINT banks used */
};

/**
 * @brief stores the last pcint state of each port
 */
static uint8_t pcint_state[PCINT_NUM_BANKS];

/**
 * @brief stores all cb and args for all defined pcint.
 */
typedef struct {
    gpio_flank_t flank;     /**< type of interrupt the flank should be triggered on */
    gpio_cb_t cb;           /**< interrupt callback */
    void *arg;              /**< optional argument */
} gpio_isr_ctx_pcint_t;

/**
 * @brief
 */
static const gpio_t pcint_mapping[] = {
#ifdef MODULE_ATMEGA_PCINT0
    ATMEGA_PCINT_MAP_PCINT0,
#endif /* PCINT0_IDX */
#ifdef MODULE_ATMEGA_PCINT1
    ATMEGA_PCINT_MAP_PCINT1,
#endif /* PCINT1_IDX */
#ifdef MODULE_ATMEGA_PCINT2
    ATMEGA_PCINT_MAP_PCINT2,
#endif /* PCINT2_IDX */
#ifdef MODULE_ATMEGA_PCINT3
    ATMEGA_PCINT_MAP_PCINT3,
#endif /* PCINT3_IDX */
};
/**
 * @brief
 */
static gpio_isr_ctx_pcint_t pcint_config[8 * PCINT_NUM_BANKS];
#endif  /* MODULE_ATMEGA_PCINTn */

#endif  /* MODULE_PERIPH_GPIO_IRQ */

int gpio_cpu_init(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode)
{
    uint8_t pin_mask = (1 << atmega_pin_num(pin));

    switch (mode) {
        case GPIO_OUT:
            _SFR_MEM8(atmega_ddr_addr(port)) |= pin_mask;
            break;
        case GPIO_IN:
            _SFR_MEM8(atmega_ddr_addr(port)) &= ~pin_mask;
            _SFR_MEM8(atmega_port_addr(port)) &= ~pin_mask;
            break;
        case GPIO_IN_PU:
            _SFR_MEM8(atmega_ddr_addr(port)) &= ~pin_mask;
            _SFR_MEM8(atmega_port_addr(port)) |= pin_mask;
            break;
        default:
            return -1;
    }

    return 0;
}

gpio_mask_t gpio_cpu_read(gpio_port_t port)
{
    return _SFR_MEM8(atmega_pin_addr(port));
}

void gpio_cpu_set(gpio_port_t port, gpio_mask_t pins)
{
    _SFR_MEM8(atmega_port_addr(port)) |= pins;
}

void gpio_cpu_clear(gpio_port_t port, gpio_mask_t pins)
{
    _SFR_MEM8(atmega_port_addr(port)) &= ~pins;
}

void gpio_cpu_toggle(gpio_port_t port, gpio_mask_t pins)
{
    /*
     * According to the data sheet, writing a one to PIN toggles the bit in
     * the PORT register, independent on the value of DDR.
     */
    _SFR_MEM8(atmega_pin_addr(port)) = _SFR_MEM8(atmega_ddr_addr(port)) & pins;
}

void gpio_cpu_write(gpio_port_t port, gpio_mask_t values)
{
    _SFR_MEM8(atmega_port_addr(port)) &= _SFR_MEM8(atmega_ddr_addr(port)) & values;
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static inline int8_t _int_num(gpio_port_t port, gpio_pin_t pin)
{
    uint8_t num;
    const gpio_t ext_ints[GPIO_EXT_INT_NUMOF] = CPU_ATMEGA_EXT_INTS;

    /* find pin in ext_ints array to get the interrupt number */
    for (num = 0; num < GPIO_EXT_INT_NUMOF; num++) {
        if (port.dev == GPIO_PORT(ext_ints[num]).dev && pin == ext_ints[num].pin) {
            return num;
        }
    }

    return -1;
}

#ifdef ENABLE_PCINT
static inline int pcint_init_int(gpio_port_t port, gpio_pin_t pin,
                                 gpio_mode_t mode, gpio_flank_t flank,
                                 gpio_cb_t cb, void *arg)
{
    int8_t offset = -1;
    uint8_t pin_num = atmega_pin_num(pin);

    for (unsigned i = 0; i < ARRAY_SIZE(pcint_mapping); i++) {
        if (port.dev != NULL && pin != GPIO_PIN_UNDEF &&
            port.dev == GPIO_PORT(pcint_mapping[i]).dev && pin == pcint_mapping[i].pin) {
            offset = i;
            break;
        }
    }

    /* if pcint was not found: return -1  */
    if (offset < 0) {
        return offset;
    }

    uint8_t bank = offset / 8;
    uint8_t bank_idx = offset % 8;
    DEBUG("PCINT enabled for bank %u offset %u\n",
          (unsigned)bank, (unsigned)offset);

    /* save configuration for pin change interrupt */
    pcint_config[offset].flank = flank;
    pcint_config[offset].arg = arg;
    pcint_config[offset].cb = cb;

    /* init gpio */
    gpio_cpu_init(port, pin, mode);
    /* configure pcint */
    cli();
    switch (bank) {
#ifdef MODULE_ATMEGA_PCINT0
        case PCINT0_IDX:
            PCMSK0 |= (1 << bank_idx);
            PCICR |= (1 << PCIE0);
            break;
#endif /* MODULE_ATMEGA_PCINT0 */
#ifdef MODULE_ATMEGA_PCINT1
        case PCINT1_IDX:
            PCMSK1 |= (1 << bank_idx);
            PCICR |= (1 << PCIE1);
            break;
#endif /* MODULE_ATMEGA_PCINT1 */
#ifdef MODULE_ATMEGA_PCINT2
        case PCINT2_IDX:
            PCMSK2 |= (1 << bank_idx);
            PCICR |= (1 << PCIE2);
            break;
#endif /* MODULE_ATMEGA_PCINT2 */
#ifdef MODULE_ATMEGA_PCINT3
        case PCINT3_IDX:
            PCMSK3 |= (1 << bank_idx);
            PCICR |= (1 << PCIE3);
            break;
#endif /* MODULE_ATMEGA_PCINT3 */
        default:
            return -1;
            break;
    }
    /* As ports are mixed in a bank (e.g. PCINT0), we can only save a single bit here! */
    uint8_t port_value = (_SFR_MEM8(atmega_pin_addr( port )));
    uint8_t pin_mask = (1 << pin_num);
    uint8_t pin_value = ((port_value & pin_mask) != 0);
    if (pin_value) {
        pcint_state[bank] |= pin_mask;
    }
    else {
        pcint_state[bank] &= ~pin_mask;
    }
    sei();
    return 0;
}
#endif /* ENABLE_PCINT */

int gpio_cpu_init_int(gpio_port_t port, gpio_pin_t pin, gpio_mode_t mode,
                      gpio_flank_t flank, gpio_cb_t cb, void *arg)
{
    int8_t int_num = _int_num(port, pin);

    /* mode not supported */
    if ((mode != GPIO_IN) && (mode != GPIO_IN_PU)) {
        return -1;
    }

    /* not a valid interrupt pin. Set as pcint instead if pcints are enabled */
    if (int_num < 0) {
#ifdef ENABLE_PCINT
        /* If pin change interrupts are enabled, enable mask and interrupt */
        return pcint_init_int(port, pin, mode, flank, cb, arg);
#else
        return -1;
#endif /* ENABLE_PCINT */
    }

    /* flank not supported */
    if (flank > GPIO_RISING) {
        return -1;
    }

    gpio_cpu_init(port, pin, mode);

    /* clear global interrupt flag */
    cli();

    /* enable interrupt number int_num */
    EIFR |= (1 << int_num);
    EIMSK |= (1 << int_num);

    /* apply flank to interrupt number int_num */
    if (int_num < 4) {
        EICRA &= ~(0x3 << (int_num * 2));
        EICRA |= (flank << (int_num * 2));
    }
    #if defined(EICRB)
    else {
        EICRB &= ~(0x3 << ((int_num % 4) * 2));
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

void gpio_cpu_irq_enable(gpio_port_t port, gpio_pin_t pin)
{
    int8_t num = _int_num(port, pin);
    EIFR |= (1 << num);
    EIMSK |= (1 << num);
}

void gpio_cpu_irq_disable(gpio_port_t port, gpio_pin_t pin)
{
    (void)port;
    EIMSK &= ~(1 << _int_num(port, pin));
}

static inline void irq_handler(uint8_t int_num)
{
    atmega_enter_isr();
    config[int_num].cb(config[int_num].arg);
    atmega_exit_isr();
}

#ifdef ENABLE_PCINT
/* inline function that is used by the PCINT ISR */
static inline void pcint_handler(uint8_t bank, uint8_t enabled_pcints)
{
    atmega_enter_isr();
    /* Find right item */
    uint8_t idx = 0;

    while (enabled_pcints > 0) {
        /* check if this pin is enabled & has changed */
        if (enabled_pcints & 0x1) {
            /* get pin from mapping (assumes 8 entries per bank!) */
            gpio_t pin = pcint_mapping[bank * 8 + idx];
            /* re-construct mask from pin */
            uint8_t pin_mask = (1 << (atmega_pin_num(pin.pin)));
            uint8_t idx_mask = (1 << idx);
            uint8_t port_value = (_SFR_MEM8(atmega_pin_addr(GPIO_PORT(pin))));
            uint8_t pin_value = ((port_value & pin_mask) != 0);
            uint8_t old_state = ((pcint_state[bank] & idx_mask) != 0);
            gpio_isr_ctx_pcint_t *conf = &pcint_config[bank * 8 + idx];
            if (old_state != pin_value) {
                pcint_state[bank] ^= idx_mask;
                if ((conf->flank == GPIO_BOTH ||
                     (pin_value && conf->flank == GPIO_RISING) ||
                     (!pin_value && conf->flank == GPIO_FALLING))) {
                    /* execute callback routine */
                    conf->cb(conf->arg);
                }
            }
        }
        enabled_pcints = enabled_pcints >> 1;
        idx++;
    }

    atmega_exit_isr();
}
#ifdef MODULE_ATMEGA_PCINT0
ISR(PCINT0_vect, ISR_BLOCK)
{
    pcint_handler(PCINT0_IDX, PCMSK0);
}
#endif /* MODULE_ATMEGA_PCINT0 */

#ifdef MODULE_ATMEGA_PCINT1
ISR(PCINT1_vect, ISR_BLOCK)
{
    pcint_handler(PCINT1_IDX, PCMSK1);
}
#endif  /* MODULE_ATMEGA_PCINT1 */

#ifdef MODULE_ATMEGA_PCINT2
ISR(PCINT2_vect, ISR_BLOCK)
{
    pcint_handler(PCINT2_IDX, PCMSK2);
}
#endif  /* MODULE_ATMEGA_PCINT2 */

#ifdef MODULE_ATMEGA_PCINT3
ISR(PCINT3_vect, ISR_BLOCK)
{
    pcint_handler(PCINT3_IDX, PCMSK3);
}
#endif  /* MODULE_ATMEGA_PCINT3 */

#endif  /* ENABLE_PCINT */

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

#endif /* MODULE_PERIPH_GPIO_IRQ */
