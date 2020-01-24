/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_ext
 * @{
 *
 * @file
 * @brief       Example GPIO extender driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef FOO_GPIO_EXT_H
#define FOO_GPIO_EXT_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Example GPIO extender device structure type
 */
typedef struct {
    const char *name;
    gpio_mask_t state;
} foo_ext_t;

/**
 * @name    Standard driver interface of the example GPIO extender
 *
 * The functions of this standard driver interface can be used directly by
 * the application.
 * @{
 */
int foo_ext_init(foo_ext_t *dev, uint8_t pin, gpio_mode_t mode);
#ifdef MODULE_PERIPH_GPIO_IRQ
int foo_ext_init_int(foo_ext_t *dev, uint8_t pin, gpio_mode_t mode,
                     gpio_flank_t flank, gpio_cb_t cb, void *arg);
void foo_ext_irq_enable(foo_ext_t *dev, uint8_t pin);
void foo_ext_irq_disable(foo_ext_t *dev, uint8_t pin);
#endif
gpio_mask_t foo_ext_read(foo_ext_t *dev);
void foo_ext_set(foo_ext_t *dev, gpio_mask_t pins);
void foo_ext_clear(foo_ext_t *dev, gpio_mask_t pins);
void foo_ext_toggle(foo_ext_t *dev, gpio_mask_t pins);
void foo_ext_write(foo_ext_t *dev, gpio_mask_t values);
/** @} */

/**
 * @name    Wrapper around the driver interface of the example GPIO extender
 *
 * The following functions map the low-level GPIO API to the driver interface.
 * This allows to use the GPIO extender via the GPIO API.
 * @{
 */
static inline int foo_gpio_ext_init(const gpio_port_t *port, gpio_pin_t pin,
                                    gpio_mode_t mode)
{
    return foo_ext_init(port->dev->dev, pin, mode);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static inline int foo_gpio_ext_init_int(const gpio_port_t *port, gpio_pin_t pin,
                                        gpio_mode_t mode, gpio_flank_t flank,
                                        gpio_cb_t cb, void *arg)
{
    return foo_ext_init_int(port->dev->dev, pin, mode, flank, cb, arg);
}

static inline void foo_gpio_ext_irq_enable(const gpio_port_t *port, gpio_pin_t pin)
{
    foo_ext_irq_enable(port->dev->dev, pin);
}

static inline void foo_gpio_ext_irq_disable(const gpio_port_t *port, gpio_pin_t pin)
{
    foo_ext_irq_disable(port->dev->dev, pin);
}
#endif

static inline gpio_mask_t foo_gpio_ext_read(const gpio_port_t *port)
{
    return foo_ext_read(port->dev->dev);
}

static inline void foo_gpio_ext_set(const gpio_port_t *port, gpio_mask_t pins)
{
    foo_ext_set(port->dev->dev, pins);
}

static inline void foo_gpio_ext_clear(const gpio_port_t *port, gpio_mask_t pins)
{
    foo_ext_clear(port->dev->dev, pins);
}

static inline void foo_gpio_ext_toggle(const gpio_port_t *port, gpio_mask_t pins)
{
    foo_ext_toggle(port->dev->dev, pins);
}

static inline void foo_gpio_ext_write(const gpio_port_t *port, gpio_mask_t values)
{
    foo_ext_write(port->dev->dev, values);
}
/** @} */

/**
 * @brief   Example GPIO extender driver structure
 */
extern const gpio_driver_t foo_gpio_ext_driver;


#ifdef __cplusplus
}
#endif

#endif /* FOO_GPIO_EXT_H */
/** @} */
