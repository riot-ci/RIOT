/*
 * Copyright (C) 2019 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_at24mac unique ID chip
 * @brief       Device driver interface for the AT24MAC I2C chip
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef AT24MAC_H
#define AT24MAC_H

#include <stdint.h>
#include "net/eui48.h"
#include "net/eui64.h"
#include "periph/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AT24MAC_ID_LEN  (16)            /**< Length of ID128 */

typedef enum __attribute__ ((__packed__)) {
    AT24MAC4XX,
    AT24MAC6XX
} at24mac_type_t;

/**
 * @brief   struct holding all params needed for device communication
 */
typedef struct {
    i2c_t i2c_dev;                      /**< I2C device      */
    uint8_t i2c_addr;                   /**< I2C address     */
    at24mac_type_t type;                /**< Device type     */
} at24mac_params_t;

/**
 * @brief   Get the unique EUI48 address from a AT24MAC4xx chip
 *
 * @param[in] idx       Index of the AT24Mac chip in the at24mac_params
 *                      array.
 * @param[out] addr     memory location to copy the address into.
 *
 * @return              0 on success, error otherwise.
 */
int at24mac_get_eui48(unsigned idx, eui48_t *addr);

/**
 * @brief   Get the unique EUI64 address from a AT24MAC6xx chip
 *
 * @param[in] idx       Index of the AT24Mac chip in the at24mac_params
 *                      array.
 * @param[out] addr     memory location to copy the address into.
 *
 * @return              0 on success, error otherwise.
 */
int at24mac_get_eui64(unsigned idx, eui64_t *addr);

/**
 * @brief   Get the unique ID from a AT24MACxxx chip
 *
 * @param[in] idx       Index of the AT24MAC chip in the at24mac_params
 *                      array.
 * @param[out] dst      memory location to copy the ID into.
 *                      Must be able to hold at least @ref AT24MAC_ID_LEN bytes.
 *
 * @return              0 on success, error otherwise.
 */
int at24mac_get_id128(unsigned idx, void *dst);

/**
 * @brief   Get the type of a AT24MACxxx chip
 *
 * @param[in] idx       Index of the AT24MAC chip in the at24mac_params
 *                      array.
 *
 * @return              The type of the device (AT24MAC4XX or AT24MAC6XX)
 */
at24mac_type_t at24mac_get_type(unsigned idx);

#ifdef __cplusplus
}
#endif

#endif /* AT24MAC_H */
/** @} */
