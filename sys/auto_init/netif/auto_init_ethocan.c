/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief   Auto initialization for ethernet-over-serial module
 *
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifdef MODULE_ETHOCAN

#include "log.h"
#include "debug.h"
#include "ethocan.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/gnrc/netif/ethernet.h"

/**
 * @brief global ethocan object, used by stdio_uart
 */
ethocan_t ethocan;

/**
 * @brief   Define stack parameters for the MAC layer thread
 * @{
 */
#define ETHOCAN_MAC_STACKSIZE (THREAD_STACKSIZE_DEFAULT + DEBUG_EXTRA_STACKSIZE)
#ifndef ETHOCAN_MAC_PRIO
#define ETHOCAN_MAC_PRIO      (GNRC_NETIF_PRIO)
#endif

#ifndef ETHOCAN_BAUDRATE
#define ETHOCAN_BAUDRATE      (115200)
#endif

/**
 * @brief   Stacks for the MAC layer threads
 */
static char _netdev_eth_stack[ETHOCAN_MAC_STACKSIZE];

void auto_init_ethocan(void)
{
    LOG_DEBUG("[auto_init_netif] initializing ethocan #0.\n");

    /* setup netdev device */
    ethocan_params_t p;
    p.uart = ETHOCAN_UART;
    p.baudrate = ETHOCAN_BAUDRATE;
    p.sense_pin = ETHOCAN_SENSE_PIN;
    ethocan_setup(&ethocan, &p);

    /* initialize netdev<->gnrc adapter state */
    gnrc_netif_ethernet_create(_netdev_eth_stack, ETHOCAN_MAC_STACKSIZE,
                               ETHOCAN_MAC_PRIO, "ethocan", (netdev_t *)&ethocan);
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_ETHOCAN */
/** @} */
