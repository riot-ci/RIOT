/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  net_gnrc_netdev   Adaption layer for GNRC on top of Netdev
 * @ingroup   net_gnrc
 * @brief     Provides the glue code for @ref net_gnrc on top of @ref drivers_netdev_api
 * @{
 *
 * @file
 * @brief     netdev-GNRC glue code interface
 *
 * This interface is supposed to provide common adaption code between the
 * low-level network device interface "netdev" and the GNRC network stack.
 *
 * GNRC sends around "gnrc_pktsnip_t" structures, but netdev can only handle
 * "struct iovec" structures when sending, or a flat buffer when receiving.
 *
 * The purpose of gnrc_netdev is to bring these two interfaces together.
 *
 * @author    Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef NET_GNRC_NETDEV_H
#define NET_GNRC_NETDEV_H

#include <assert.h>
#include <stdint.h>

#include "kernel_types.h"
#include "net/netdev.h"
#include "net/gnrc.h"
#include "net/gnrc/mac/types.h"
#include "net/ieee802154.h"
#include "net/gnrc/mac/mac.h"
#ifdef MODULE_GNRC_MAC
#include "net/csma_sender.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default priority for adaption layer's threads
 */
#ifndef GNRC_NETDEV_MAC_PRIO
#define GNRC_NETDEV_MAC_PRIO    (THREAD_PRIORITY_MAIN - 5)
#endif

/**
 * @brief   Type for @ref msg_t if device fired an event
 */
#define NETDEV_MSG_TYPE_EVENT 0x1234

/**
 * @brief Structure holding GNRC netdev adapter state
 *
 * This structure is supposed to hold any state parameters needed
 * to use a netdev device from GNRC.
 *
 * It can be extended
 */
typedef struct gnrc_netdev {
    /**
     * @brief Send a pktsnip using this device
     *
     * This function should convert the pktsnip into a format
     * the underlying device understands and send it.
     */
    int (*send)(struct gnrc_netdev *dev, gnrc_pktsnip_t *snip);

    /**
     * @brief Receive a pktsnip from this device
     *
     * This function should receive a raw frame from the underlying
     * device and convert it into a pktsnip while adding a netif header
     * and possibly marking out higher-layer headers.
     */
    gnrc_pktsnip_t * (*recv)(struct gnrc_netdev *dev);

    /**
     * @brief netdev handle this adapter is working with
     */
    netdev_t *dev;

    /**
     * @brief PID of this adapter for netapi messages
     */
    kernel_pid_t pid;

} gnrc_netdev_t;

/**
 * @brief Initialize GNRC netdev handler thread
 *
 * @param[in] stack         ptr to preallocated stack buffer
 * @param[in] stacksize     size of stack buffer
 * @param[in] priority      priority of thread
 * @param[in] name          name of thread
 * @param[in] gnrc_netdev  ptr to netdev device to handle in created thread
 *
 * @return pid of created thread
 * @return KERNEL_PID_UNDEF on error
 */
kernel_pid_t gnrc_netdev_init(char *stack, int stacksize, char priority,
                               const char *name, gnrc_netdev_t *gnrc_netdev);

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_NETDEV_H */
/** @} */
