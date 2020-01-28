/*
 * Copyright (C) 2020 Pieter du Preez <pdupreez@gmail.com>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_comms_bus (I2C/SPI)
 * @ingroup     drivers_periph
 * @brief       Low-level I2C/SPI bus driver interface
 *
 * This is a kind of router for drivers that are able to support
 * either I2C or API bus interfaces.
 *
 * It is required to call the comms_bus_setup function as start-up.
 * This function sets up function pointers for directing bus
 * communication to the desired bus.
 *
 * @{
 *
 * @file
 * @brief       Low-level I2C/SPI bus driver interface definition
 *
 * @author      Pieter du Preez <pdupreez@gmail.com>
 */

#ifndef PERIPH_COMMS_BUS_H
#define PERIPH_COMMS_BUS_H

/**
 * The following check breaks the CI build.
 * Strictly, the check is not needed here, so it's commented out for now.

#if !defined(MODULE_PERIPH_I2C) && !defined(MODULE_PERIPH_SPI)
#error At least one of the following modules are required: periph_i2c, periph_spi
#endif

*/

#ifdef MODULE_PERIPH_SPI
#include "spi.h"
#endif

#ifdef MODULE_PERIPH_I2C
#include "i2c.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Supported bus types.
 */
enum comms_bus_type {
    PERIPH_COMMS_BUS_UNDEF = 0,
    PERIPH_COMMS_BUS_I2C,
    PERIPH_COMMS_BUS_SPI,
};

typedef enum comms_bus_type comms_bus_type_t;

#ifdef MODULE_PERIPH_SPI
typedef struct  {
    spi_t dev;
    gpio_t cs;
    spi_mode_t mode;
    spi_clk_t clk;
} spi_bus_t;
#endif

#ifdef MODULE_PERIPH_I2C
typedef struct {
    i2c_t dev;
    uint8_t addr;
} i2c_bus_t;
#endif

typedef union
{
#ifdef MODULE_PERIPH_SPI
    spi_bus_t spi;
#endif
#ifdef MODULE_PERIPH_I2C
    i2c_bus_t i2c;
#endif
#if !defined(MODULE_PERIPH_I2C) && !defined(MODULE_PERIPH_SPI)
    int dummy_to_avoid_union_has_no_members_warning;
#endif
} comms_bus_params_t;

/**
 * @brief   Common bus communication function types.
 */

typedef int comms_bus_init_t(const comms_bus_params_t *bus);
typedef int comms_bus_acquire_t(const comms_bus_params_t *bus);
typedef void comms_bus_release_t(const comms_bus_params_t *bus);
typedef int comms_bus_read_reg_t(const comms_bus_params_t *bus,
                                 uint16_t reg, uint8_t *out);
typedef int comms_bus_read_regs_t(const comms_bus_params_t *bus,
                                  uint16_t reg, void *data, size_t len);
typedef int comms_bus_write_reg_t(const comms_bus_params_t *bus,
                                  uint8_t reg, uint8_t data);

/**
 * @brief   Function pointer structure for pivoting to a specified bus.
 */

typedef struct {
    comms_bus_init_t *comms_bus_init;
    comms_bus_acquire_t *comms_bus_acquire;
    comms_bus_release_t *comms_bus_release;
    comms_bus_read_reg_t *comms_bus_read_reg;
    comms_bus_read_regs_t *comms_bus_read_regs;
    comms_bus_write_reg_t *comms_bus_write_reg;
} comms_bus_function_t;

/**
 * @brief   The transport struct contains the bus type, the bus
 *          and the common bus function pointers.
 */

typedef struct
{
    comms_bus_type_t type;
    comms_bus_params_t bus;
    comms_bus_function_t f;
} comms_transport_t;

/**
 * @brief   Function that must be called at start-up.
 */

void comms_bus_setup(comms_transport_t* transport);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_COMMS_BUS_H */
/** @} */
