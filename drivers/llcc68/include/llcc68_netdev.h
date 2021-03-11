/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_llcc68
 * @{
 * @file
 * @brief       Netdev driver definitions for LLCC68 driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LLCC68_NETDEV_H
#define LLCC68_NETDEV_H

#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to the netdev device driver struct
 */
extern const netdev_driver_t llcc68_driver;

#ifdef __cplusplus
}
#endif

#endif /* LLCC68_NETDEV_H */
/** @} */
