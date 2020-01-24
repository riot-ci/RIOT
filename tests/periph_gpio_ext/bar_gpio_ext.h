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

#ifndef BAR_GPIO_EXT_H
#define BAR_GPIO_EXT_H

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
} bar_ext_t;

/**
 * @name    Standard driver interface of the example GPIO extender
 *
 * The functions of this standard driver interface can be used directly by
 * the application.
 * @{
 */
int bar_ext_init(bar_ext_t *dev, uint8_t pin, gpio_mode_t mode);
#ifdef MODULE_PERIPH_GPIO_IRQ
int bar_ext_init_int(bar_ext_t *dev, uint8_t pin, gpio_mode_t mode,
                     gpio_flank_t flank, gpio_cb_t cb, void *arg);
void bar_ext_irq_enable(bar_ext_t *dev, uint8_t pin);
void bar_ext_irq_disable(bar_ext_t *dev, uint8_t pin);
#endif
gpio_mask_t bar_ext_read(bar_ext_t *dev);
void bar_ext_set(bar_ext_t *dev, gpio_mask_t pins);
void bar_ext_clear(bar_ext_t *dev, gpio_mask_t pins);
void bar_ext_toggle(bar_ext_t *dev, gpio_mask_t pins);
void bar_ext_write(bar_ext_t *dev, gpio_mask_t values);
/** @} */

/**
 * @name    Wrapper around the driver interface of the example GPIO extender
 *
 * The following functions map the low-level GPIO API to the driver interface.
 * This allows to use the GPIO extender via the GPIO API.
 * @{
 */
static inline int bar_gpio_ext_init(const gpio_port_t *port, gpio_pin_t pin,
                                    gpio_mode_t mode)
{
    return bar_ext_init(port->dev->dev, pin, mode);
}

#ifdef MODULE_PERIPH_GPIO_IRQ
static inline int bar_gpio_ext_init_int(const gpio_port_t *port, gpio_pin_t pin,
                                        gpio_mode_t mode, gpio_flank_t flank,
                                        gpio_cb_t cb, void *arg)
{
    return bar_ext_init_int(port->dev->dev, pin, mode, flank, cb, arg);
}

static inline void bar_gpio_ext_irq_enable(const gpio_port_t *port, gpio_pin_t pin)
{
    bar_ext_irq_enable(port->dev->dev, pin);
}

static inline void bar_gpio_ext_irq_disable(const gpio_port_t *port, gpio_pin_t pin)
{
    bar_ext_irq_disable(port->dev->dev, pin);
}
#endif

static inline gpio_mask_t bar_gpio_ext_read(const gpio_port_t *port)
{
    return bar_ext_read(port->dev->dev);
}

static inline void bar_gpio_ext_set(const gpio_port_t *port, gpio_mask_t pins)
{
    bar_ext_set(port->dev->dev, pins);
}

static inline void bar_gpio_ext_clear(const gpio_port_t *port, gpio_mask_t pins)
{
    bar_ext_clear(port->dev->dev, pins);
}

static inline void bar_gpio_ext_toggle(const gpio_port_t *port, gpio_mask_t pins)
{
    bar_ext_toggle(port->dev->dev, pins);
}

static inline void bar_gpio_ext_write(const gpio_port_t *port, gpio_mask_t values)
{
    bar_ext_write(port->dev->dev, values);
}

/** @} */

/**
 * @brief   Example GPIO extender driver structure
 */
extern const gpio_driver_t bar_gpio_ext_driver;


#ifdef __cplusplus
}
#endif

#endif /* BAR_GPIO_EXT_H */
/** @} */
