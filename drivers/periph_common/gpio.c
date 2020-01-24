/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Common GPIO driver functions/definitions
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifdef MODULE_PERIPH_GPIO_EXT

#include "periph_cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_EXTEND_GPIO
#include "gpio_ext_conf.h"
#endif

/* the CPU low level GPIO driver */
const gpio_driver_t gpio_cpu_driver = {
    .init = gpio_cpu_init,
#ifdef MODULE_PERIPH_GPIO_IRQ
    .init_int = gpio_cpu_init_int,
    .irq_enable = gpio_cpu_irq_enable,
    .irq_disable = gpio_cpu_irq_disable,
#endif /* MODULE_PERIPH_GPIO_IRQ */
    .read = gpio_cpu_read,
    .set = gpio_cpu_set,
    .clear = gpio_cpu_clear,
    .toggle = gpio_cpu_toggle,
    .write = gpio_cpu_write,
};

const gpio_port_t gpio_ports[] = {
#if defined(MODULE_PERIPH_GPIO)
    GPIO_CPU_PORTS
#endif
#if defined(MODULE_EXTEND_GPIO) && defined(GPIO_EXT_PORTS)
    GPIO_EXT_PORTS
#endif
};

int gpio_port_numof(void)
{
    return ARRAY_SIZE(gpio_ports);
}

int gpio_port(gpio_t gpio)
{
    for (unsigned i = 0; i < ARRAY_SIZE(gpio_ports); i++) {
        if (&gpio_ports[i] == gpio.port) {
            return i;
        }
    }
    return 0;
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_PERIPH_GPIO_EXT */
