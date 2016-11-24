/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup auto_init
 * @{
 * @file
 * @brief       initializes stm32 can device
 *
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @}
 */

#ifdef MODULE_CAN_STM32
#include "can/device.h"
#include "candev_stm32_params.h"

#define CANDEV_STM32_NUMOF ((sizeof(candev_stm32_params) / sizeof(candev_stm32_params[0])))

#ifndef CANDEV_STM32_STACKSIZE
#define CANDEV_STM32_STACKSIZE (THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

#ifndef CANDEV_STM32_BASE_PRIORITY
#define CANDEV_STM32_BASE_PRIORITY (THREAD_PRIORITY_MAIN - CANDEV_STM32_NUMOF - 2)
#endif

static candev_dev_t candev_dev_stm32[CANDEV_STM32_NUMOF];
static char _can_stm32_stacks[CANDEV_STM32_NUMOF][CANDEV_STM32_STACKSIZE];
static candev_stm32_t candev_stm32[CANDEV_STM32_NUMOF];

void auto_init_can_stm32(void) {

    for (size_t i = 0; i < CANDEV_STM32_NUMOF; i++) {
        candev_stm32_init(&candev_stm32[i], &candev_stm32_conf[i]);
        candev_dev_stm32[i].dev = (candev_t *)&candev_stm32[i];
        candev_dev_stm32[i].name = candev_stm32_params[i].name;
#ifdef MODULE_CAN_TRX
        candev_dev_stm32[i].trx = candev_stm32_params[i].trx;
#endif
#ifdef MODULE_CAN_PM
        candev_dev_stm32[i].rx_inactivity_timeout = candev_stm32_params[i].rx_inactivity_timeout;
        candev_dev_stm32[i].tx_wakeup_timeout = candev_stm32_params[i].tx_wakeup_timeout;
#endif

        can_device_init(_can_stm32_stacks[i], CANDEV_STM32_STACKSIZE, CANDEV_STM32_BASE_PRIORITY + i,
                        candev_stm32_params[i].name, &candev_dev_stm32[i]);
    }
}
#else
typedef int dont_be_pedantic;
#endif
