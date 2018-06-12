/*
 * Copyright (C) 2010,2015 Freie Universität Berlin
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013-2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_auto_init Auto-initialization
 * @ingroup     sys
 * @brief       Auto initialize modules
 *
 * This feature can be enabled in any application by adding the `auto_init`
 * module to the application `Makefile`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * USEMODULE += auto_init
 * ~~~~~~~~~~~~~~~~~~~~~~~
 *
 * `auto_init` initializes any included module that provides
 * auto-initialization capabilities.
 *
 * Drivers or cpu peripherals that provides a @ref drivers_saul adaption and
 * @ref drivers_netdev can be initialized automatically using the `auto_init`
 * module.
 *
 * For regular device drivers such as sensors, actuators, network devices
 * (@ref drivers), the default initialization parameters are taken from the
 * `DRIVER_params.h` file provided by the driver implementation.
 *
 * The default initialization parameters can be overriden by the application in
 * several ways (examples with the @ref drivers_bmp180 oversampling parameter
 * `BMP180_PARAM_OVERSAMPLING`):
 *
 *  - by passing them via the `CFLAGS` variable on the build command line`:
 *
 * ```
 * CFLAGS=-DBMP180_PARAM_OVERSAMPLING=1 USEMODULE=bmp180 make BOARD=arduino-zero -C examples/default
 * ```
 *
 * - by just setting the `CFLAGS` variable in the application `Makefile`:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~ {.mk}
 * CFLAGS +=-DBMP180_PARAM_OVERSAMPLING=1
 * ~~~~~~~~~~~~~~~~~~~~~~~
 *
 * - by copying the `bmp180_params.h` header to the application directory and
 * editing it there with the desired values (not recommended). This file will
 * be included first and, thanks to header guards, the one from the driver
 * implementation will be skipped.
 *
 * From low-level CPU peripheral, the default initialization parameters are
 * defined in each board configuration that supports them.
 *
 * The modules will be initialized in the context of the main thread right
 * before the main function gets called. Be aware that most modules expect to
 * be initialized only once, so do not call a module's init function when using
 * `auto_init` unless you know what you're doing.
 */

/**
 * @defgroup    sys_auto_init_saul SAUL drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of SAUL drivers
 *
 * @see @ref drivers_saul, @ref sys_auto_init
 */

/**
 * @defgroup    sys_auto_init_gnrc_netif GNRC netif drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of
 * [network device drivers](@ref drivers_netdev) via GNRC
 *
 * @see @ref net_gnrc_netif, @ref sys_auto_init
 */

/**
 * @{
 *
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef AUTO_INIT_H
#define AUTO_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes all high level modules that do not require parameters for
 *        initialization or uses default values.
 *
 *        This function gets called - if not explicitely disabled - by @ref
 *        kernel_init right before jumping into @e main.
 */
void auto_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AUTO_INIT_H */
