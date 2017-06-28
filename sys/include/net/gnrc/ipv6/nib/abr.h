/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 * @{
 *
 * @file
 * @brief
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef NET_GNRC_IPV6_NIB_ABR_H
#define NET_GNRC_IPV6_NIB_ABR_H

#include "net/ipv6/addr.h"
#include "net/gnrc/ipv6/nib/conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#if     (GNRC_IPV6_NIB_CONF_6LBR && GNRC_IPV6_NIB_CONF_MULTIHOP_P6C) || defined(DOXYGEN)
int gnrc_ipv6_nib_abr_add(ipv6_addr_t *addr);
void gnrc_ipv6_nib_abr_del(ipv6_addr_t *addr);
#endif  /* (GNRC_IPV6_NIB_CONF_6LBR && GNRC_IPV6_NIB_CONF_MULTIHOP_P6C) || defined(DOXYGEN) */

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_IPV6_NIB_ABR_H */
/** @} */
