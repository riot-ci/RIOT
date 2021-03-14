/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     sys_auto_init_gnrc_netif
 * @{
 *
 * @file
 * @brief       Auto initialization for LLCC68 LoRa interfaces
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <assert.h>

#include "log.h"
#include "board.h"
#include "net/gnrc/netif/lorawan_base.h"
#include "net/gnrc/netif/raw.h"
#include "net/gnrc.h"

#include "llcc68.h"
#include "llcc68_params.h"

/**
 * @brief   Calculate the number of configured LLCC68 devices
 */
#define LLCC68_NUMOF                ARRAY_SIZE(llcc68_params)

/**
 * @brief   Define stack parameters for the MAC layer thread
 */
#define LLCC68_STACKSIZE            (THREAD_STACKSIZE_DEFAULT)
#ifndef LLCC68_PRIO
#define LLCC68_PRIO                 (GNRC_NETIF_PRIO)
#endif

/**
 * @brief   Allocate memory for device descriptors, stacks, and GNRC adaption
 */
static llcc68_t llcc68_devs[LLCC68_NUMOF];
static char llcc68_stacks[LLCC68_NUMOF][LLCC68_STACKSIZE];
static gnrc_netif_t _netif[LLCC68_NUMOF];

void auto_init_llcc68(void)
{
    for (unsigned i = 0; i < LLCC68_NUMOF; ++i) {
        LOG_DEBUG("[auto_init_netif] initializing llcc68 #%u\n", i);
        llcc68_setup(&llcc68_devs[i], &llcc68_params[i], i);
        if (IS_USED(MODULE_GNRC_NETIF_LORAWAN)) {
            /* Currently only one lora device is supported */
            assert(LLCC68_NUMOF == 1);

            gnrc_netif_lorawan_create(&_netif[i], llcc68_stacks[i],
                                      LLCC68_STACKSIZE, LLCC68_PRIO,
                                      "llcc68", (netdev_t *)&llcc68_devs[i]);
        }
        else {
            gnrc_netif_raw_create(&_netif[i], llcc68_stacks[i],
                                  LLCC68_STACKSIZE, LLCC68_PRIO,
                                  "llcc68", (netdev_t *)&llcc68_devs[i]);
        }
    }
}
/** @} */
