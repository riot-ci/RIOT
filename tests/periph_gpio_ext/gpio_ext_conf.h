/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests_periph_gpio_ext
 *
 * @{
 *
 * @file
 * @brief       Example GPIO extender configuration
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef GPIO_EXT_CONF_H
#define GPIO_EXT_CONF_H

#include <stddef.h>

#include "periph/gpio.h"

#include "foo_gpio_ext.h" /* include header files of GPIO extender drivers */
#include "bar_gpio_ext.h" /* include header files of GPIO extender drivers */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Defined the extender devices
 */
static foo_ext_t foo_ext_1 = { .name = "foo1" };
static foo_ext_t foo_ext_2 = { .name = "foo2" };
static bar_ext_t bar_ext   = { .name = "bar" };

/**
 * @brief   Define extender device structures for all extenders
 */
static const gpio_dev_t foo_ext_dev_1 = { .dev = &foo_ext_1, .driver = &foo_gpio_ext_driver };
static const gpio_dev_t foo_ext_dev_2 = { .dev = &foo_ext_2, .driver = &foo_gpio_ext_driver };
static const gpio_dev_t bar_ext_dev   = { .dev = &bar_ext  , .driver = &bar_gpio_ext_driver };

/**
 * @brief   GPIO expansion default list if not defined
 */
#define GPIO_EXT_PORTS \
    { .dev = &foo_ext_dev_1 }, \
    { .dev = &foo_ext_dev_2 }, \
    { .dev = &bar_ext_dev   },

#ifdef __cplusplus
}
#endif

#endif /* GPIO_EXT_CONF_H */
/** @} */
