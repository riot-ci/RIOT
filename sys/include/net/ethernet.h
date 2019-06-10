/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_ethernet Ethernet
 * @ingroup     net
 * @brief       Provides Ethernet header and helper functions
 * @{
 *
 * @file
 * @brief       Definitions for Ethernet
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef NET_ETHERNET_H
#define NET_ETHERNET_H

#include <stdint.h>

#include "net/ethernet/hdr.h"
#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETHERNET_DATA_LEN       (1500)  /**< maximum number of bytes in payload */
#define ETHERNET_FCS_LEN        (4)     /**< number of bytes in the FCS
                                         *  (frame check sequence) */

/**
 * @brief maximum number of bytes in an ethernet frame (without FCS)
 */
#define ETHERNET_FRAME_LEN      (ETHERNET_DATA_LEN + sizeof(ethernet_hdr_t))
#define ETHERNET_MIN_LEN        (64)    /**< minimum number of bytes in an
                                         * ethernet frame (with FCF) */

/**
 * @brief maximum number of bytes in an ethernet frame (with FCF)
 */
#define ETHERNET_MAX_LEN        (ETHERNET_FRAME_LEN + ETHERNET_FCS_LEN)

/**
 * @brief Set the locally administrated bit in the ethernet address.
 *
 * @see ieee 802-2001 section 9.2
 *
 * @param   addr    ethernet address
 */
static inline void ethernet_set_locally_admin(uint8_t *addr)
{
    addr[0] |= ETHERNET_ADDR_LOCAL_ADMIN;
}

/**
 * @brief Clear the group address bit to signal the address as individual
 * address
 *
 * @see ieee 802-2001 section 9.2
 *
 * @param   addr    ethernet address
 */
static inline void ethernet_set_individual_addr(uint8_t *addr)
{
    addr[0] &= ~ETHERNET_ADDR_GROUP;
}

#ifdef __cplusplus
}
#endif

#endif /* NET_ETHERNET_H */
/**
 * @}
 */
