/*
 * Copyright (C) 2021 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_utils   Network helper functions
 * @ingroup     net
 * @brief       Common network helper functions
 * @{
 *
 * @file
 * @brief       Common network interface API definitions
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef NET_UTILS_H
#define NET_UTILS_H

#include <stdint.h>
#include <stddef.h>
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Parse an IP address / hostname string.
 *          If the @ref net_sock_dns module is used, this will
 *          attempt to resolve hostnames via DNS.
 *
 * @param[in]   hostname    IP address string or hostname
 * @param[out]  addr        IP address of the host
 * @param[out]  netif       Interface if address is link-local
 *
 * @return  0 on success, error otherwise
 */
int netutil_parse_hostname(const char *hostname, ipv6_addr_t *addr, netif_t **netif);

#ifdef __cplusplus
}
#endif

#endif /* NET_UTILS_H */
/** @} */
