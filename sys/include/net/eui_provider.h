/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_eui_provider    IEEE EUI-48/64 provider
 * @ingroup     net
 * @brief       MAC address management
 * @{
 *
 * @file
 * @brief   EUI-48 and EUI-64 address provider
 *
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef NET_EUI_PROVIDER_H
#define NET_EUI_PROVIDER_H

#include "net/eui48.h"
#include "net/eui64.h"
#include "net/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Will match any device index
 */
#define NETDEV_INDEX_ANY    (0xFF)

/**
 * @brief   Function for providing a EUI-48 to a device
 *
 * @param[in]   arg     Optional argument provided by eui48_conf_t
 * @param[out]  addr    Destination pointer for the EUI-48 address
 *
 * @return      0 on success, next provider in eui48_conf_t will be
 *              used otherwise.
 *              Will fall back to @see luid_get_eui48 eventually.
 */
typedef int (*netdev_get_eui48_cb_t)(const void *arg, eui48_t *addr);

/**
 * @brief   Function for providing a EUI-64 to a device
 *
 * @param[in]   arg     Optional argument provided by eui64_conf_t
 * @param[out]  addr    Destination pointer for the EUI-64 address
 *
 * @return      0 on success, next provider in eui64_conf_t will be
 *              used otherwise.
 *              Will fall back to @see luid_get_eui64 eventually.
 */
typedef int (*netdev_get_eui64_cb_t)(const void *arg, eui64_t *addr);

/**
 * @brief Structure to hold providers for EUI-48 addresses
 */
typedef struct {
    netdev_get_eui48_cb_t provider; /**< function to provide an EUI-48     */
    const void *arg;                /**< argument to the provider function */
    netdev_type_t type;             /**< device type to match              */
    uint8_t index;                  /**< device index to match             */
} eui48_conf_t;

/**
 * @brief Structure to hold providers for EUI-64 addresses
 */
typedef struct {
    netdev_get_eui64_cb_t provider; /**< function to provide an EUI-64     */
    const void *arg;                /**< argument to the provider function */
    netdev_type_t type;             /**< device type to match              */
    uint8_t index;                  /**< device index to match             */
} eui64_conf_t;

/**
 * @brief Generates an EUI-48 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant address
 *       by implementing a EUI-48 provider function.
 *       If no such function is available, this will fall back to
 *       @ref luid_get_eui48.
 *
 * @param[in] netdev    The network device for which the address is
 *                      generated.
 * @param[out] addr     The generated EUI-48 address
 *
 */
void netdev_eui48_get(netdev_t *netdev, eui48_t *addr);

/**
 * @brief Generates an EUI-64 address for the netdev interface.
 *
 * @note It is possible to supply a board-specific, constant address
 *       by implementing a EUI-64 provider function.
 *       If no such function is available, this will fall back to
 *       @ref luid_get_eui64.
 *
 * @param[in] netdev    The network device for which the address is
 *                      generated.
 * @param[out] addr     The generated EUI-64 address
 *
 */
void netdev_eui64_get(netdev_t *netdev, eui64_t *addr);

/**
 * @brief   Get a short unicast address from an EUI-64
 *
 * The resulting address is built from the provided long address.
 * The last two bytes of the long address will be used as the short
 * address with the first bit cleared.
 *
 * @param[in]  addr_long    the address to base the short address on
 * @param[out] addr_short   memory location to copy the address into.
 */
static inline void eui_short_from_eui64(eui64_t *addr_long,
                                        network_uint16_t *addr_short)
{
    /* https://tools.ietf.org/html/rfc4944#section-12 requires the first bit to
     * 0 for unicast addresses */
    addr_short->u8[0] = addr_long->uint8[6] & 0x7F;
    addr_short->u8[1] = addr_long->uint8[7];
}

#ifdef __cplusplus
}
#endif

#endif /* NET_EUI_PROVIDER_H */
/** @} */
