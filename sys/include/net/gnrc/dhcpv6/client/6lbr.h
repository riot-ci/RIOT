/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_dhcpv6_client_6lbr  DHCPv6 client for 6LoWPAN border routers
 * @ingroup     net_dhcpv6_client
 * @brief       DHCPv6 client bootstrapping for 6LoWPAN border routers
 * @{
 *
 * @file
 * @brief   DHCPv6 client on 6LoWPAN border router definitions
 *
 * @author  Martine S. Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_DHCPV6_CLIENT_6LBR_H
#define NET_GNRC_DHCPV6_CLIENT_6LBR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Identifier of the upstream interface
 *
 * Leave 0 (default) to let the client pick the first non-6LoWPAN interface it
 * finds
 */
#ifndef CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM
#define CONFIG_GNRC_DHCPV6_CLIENT_6LBR_UPSTREAM (0)
#endif

void gnrc_dhcpv6_client_6lbr_init(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_DHCPV6_CLIENT_6LBR_H */
/** @} */
