/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_saul_reg SAUL registry
 * @ingroup     sys
 * @brief       Global sensor/actuator registry for SAUL devices
 *
 * @see @ref drivers_saul
 *
 * @{
 *
 * @file
 * @brief       SAUL registry interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef SAUL_REG_H
#define SAUL_REG_H

#include <stdint.h>

#include "saul.h"
#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SAUL_HAVE_CTXT_LIST_T
/**
 * @brief   SAUL context list type
 */
#define SAUL_HAVE_CTXT_LIST_T
typedef uint16_t saul_ctxt_list_t;
#endif

/**
 * @brief   SAUL registry entry
 */
typedef struct saul_reg {
    struct saul_reg *next;          /**< pointer to the next device */
    void *dev;                      /**< pointer to the device descriptor */
    const char *name;               /**< string identifier for the device */
    saul_driver_t const *driver;    /**< the devices read callback */
    saul_ctxt_list_t ctxtlist;      /**< bitfield of valid context numbers */
} saul_reg_t;

/**
 * @brief   Additional data to collect for each entry
 */
typedef struct {
    const char *name;           /**< string identifier for a device */
} saul_reg_info_t;

/**
 * @brief   Export the SAUL registry as global variable
 */
extern saul_reg_t *saul_reg;

/**
 * @brief   SAUL pointer and context
 *
 * @note    The SAUL registry is comprised of real members and imaginary
 *          duplicates. If a device is added to the registry, it will add
 *          a number of duplicates without using additional system resources.
 *          This is intended to make support easier for devices with an array
 *          of similar inputs/outputs (such as GPIO expanders, or analog muxes)
 */
typedef struct saul_ctxt_ptr {
    struct saul_reg *reg;    /**< pointer to device in SAUL registry */
    uint8_t ctxt;            /**< context supplied to device */
} saul_ctxt_ptr_t;

/**
 * @brief   Register a device with the SAUL registry
 *
 * @note    Make sure the registry entry the @p dev pointer is pointing to
 *          resides in some persistent memory location and not on some position
 *          on the stack where it will be overwritten...
 *
 * @param[in] dev       pointer to a pre-populated registry entry
 *
 * @return      0 on success
 * @return      -ENODEV on invalid entry given
 */
int saul_reg_add(saul_reg_t *dev);

/**
 * @brief   Unregister a device from the SAUL registry
 *
 * @param[in] dev       pointer to a registry entry
 *
 * @return      0 on success
 * @return      -ENODEV if device was not found in the registry
 */
int saul_reg_rm(saul_reg_t *dev);

/**
 * @brief   Find a device by it's position in the registry
 *
 * @param[in] pos       position to look up
 *
 * @return      context+pointer to the device at position specified by @p pos
 * @return      .reg = NULL if no device is registered at that position
 */
saul_ctxt_ptr_t saul_reg_find_nth(int pos);

/**
 * @brief   Find the first device of the given type in the registry
 *
 * @param[in] type      device type to look for
 *
 * @return      pointer to the first device matching the given type
 * @return      NULL if no device of that type could be found
 */
saul_reg_t *saul_reg_find_type(uint8_t type);

/**
 * @brief   Find a device by its name
 *
 * @param[in] name      the name to look for
 *
 * @return      pointer to the first device matching the given name
 * @return      NULL if no device with that name could be found
 */
saul_reg_t *saul_reg_find_name(const char *name);

/**
 * @brief   Read data from the given device
 *
 * @param[in] dev     pointer to device to read from
 * @param[in] ctxt    context for device to write to
 * @param[out] res    location to store the results in
 *
 * @return      the number of data elements read to @p res [1-3]
 * @return      -ENODEV if given device is invalid
 * @return      -ENOTSUP if read operation is not supported by the device
 * @return      -ECANCELED on device errors
 */
int saul_reg_read(saul_reg_t *dev, uint8_t ctxt, phydat_t *res);

/**
 * @brief   Write data to the given device
 *
 * @param[in] dev        pointer to device to write to
 * @param[in] context    context for device to write to
 * @param[in] data       data to write to the device
 *
 * @return      the number of data elements processed by the device
 * @return      -ENODEV if given device is invalid
 * @return      -ENOTSUP if read operation is not supported by the device
 * @return      -ECANCELED on device errors
 */
int saul_reg_write(saul_reg_t *dev, uint8_t ctxt, phydat_t *data);

#ifdef __cplusplus
}
#endif

#endif /* SAUL_REG_H */
/** @} */
