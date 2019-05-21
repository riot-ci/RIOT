/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sock_config SOCK compile configurations
 * @ingroup     net_sock
 * @ingroup     config
 * @brief
 * @{
 *
 * @file
 * @brief   Configuration macros for @ref net_sock
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#ifndef NET_SOCK_CONFIG_H
#define NET_SOCK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name helper definitions
 * @{
 */

/**
 * @brief maximum length of the scheme part for sock_urlsplit.
 *
 * Ensures a hard limit on the string iterator
 * */
#ifndef SOCK_SCHEME_MAXLEN
#define SOCK_SCHEME_MAXLEN      (16U)
#endif

/**
 * @brief maximum length of host:port part for sock_urlsplit()
 */
#ifndef SOCK_HOSTPORT_MAXLEN
#define SOCK_HOSTPORT_MAXLEN    (64U)
#endif

/**
 * @brief maximum length path for sock_urlsplit()
 */
#ifndef SOCK_URLPATH_MAXLEN
#define SOCK_URLPATH_MAXLEN     (64U)
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NET_SOCK_CONFIG_H */
/** @} */
