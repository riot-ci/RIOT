/*
 * Copyright (C) 2019 Juergen Fitschen <me@jue.yt>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_ethocan
 * @{
 * @file
 * @brief       Default configuration for the Ethernet-over-CAN driver
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef ETHOCAN_PARAMS_H
#define ETHOCAN_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the Ethernet-over-CAN driver
 * @{
 */
#ifndef ETHOCAN_PARAM_UART
#define ETHOCAN_PARAM_UART         (UART_DEV(0))
#endif
#ifndef ETHOCAN_PARAM_BAUDRATE
#define ETHOCAN_PARAM_BAUDRATE     (115200)
#endif
#ifndef ETHOCAN_PARAM_SENSE_PIN
#define ETHOCAN_PARAM_SENSE_PIN    (GPIO_PIN(0, 0))
#endif

#ifndef ETHOCAN_PARAMS
#define ETHOCAN_PARAMS             { .uart = ETHOCAN_PARAM_UART,         \
                                     .baudrate = ETHOCAN_PARAM_BAUDRATE, \
                                     .sense_pin = ETHOCAN_PARAM_SENSE_PIN }
#endif
/**@}*/

/**
 * @brief   Ethernet-over-CAN configuration
 */
static const ethocan_params_t ethocan_params[] =
{
    ETHOCAN_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* ETHOCAN_PARAMS_H */
/** @} */
