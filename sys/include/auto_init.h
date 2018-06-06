/*
 * Copyright (C) 2010,2015 Freie Universität Berlin
 * Copyright (C) 2010 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_auto_init Auto-init
 * @ingroup     sys
 * @brief       Auto initialize modules
 *
 * This feature can be enabled in any application by adding the `auto_init`
 * module to the application Makefile:
 *
 * ```mk
 * USEMODULE += auto_init
 * ```
 *
 * `auto_init` initializes any included module that provides auto
 * initialization capabilities.
 *
 * Drivers or cpu peripherals that provides a SAUL adaption and net interfaces
 * can be initialized automatically with auto_init.
 *
 * For high-level device drivers (@ref drivers), the default initialization
 * parameters are taken from the `DRIVERS_params.h` files provided by the
 * driver implementation.
 *
 * From low-level CPU peripheral, the default initialization parameters are
 * defined in each board configuration that supports them.
 *
 * The modules will be initialized in the context of the main thread right
 * before the main function gets called. Be aware that most modules expect to
 * be initialized only once, so do not call a module's init function when using
 * auto_init unless you know what you're doing.
 */

/**
 * @defgroup    sys_auto_init_saul SAUL drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of SAUL drivers.
 *
 * This module contains all high-level drivers (sensor or actuators) that
 * support auto-initialization via SAUL.
 */

/**
 * @defgroup    sys_auto_init_gnrc_netif GNRC netif drivers auto-initialization
 * @ingroup     sys_auto_init
 * @brief       Provides auto-initialization of Netif drivers via GNRC.
 *
 * This module contains all netif drivers that support auto-initialization.
 */

/**
 * @{
 *
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
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
