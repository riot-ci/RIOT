/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at24cxxx
 * @{
 *
 * @file
 * @brief       Default configuration for AT24CXXX
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT24CXXX_PARAMS_H
#define AT24CXXX_PARAMS_H

#include "board.h"
#include "at24cxxx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Set default configuration parameters for the AT24CXXX driver
 * @{
 */
#ifndef AT24CXXX_PARAM_I2C
#define AT24CXXX_PARAM_I2C             (I2C_DEV(0)) /**< I2C bus the EEPROM is connected to */
#endif
#ifndef AT24CXXX_PARAM_ADDR
#define AT24CXXX_PARAM_ADDR            (AT24CXXX_DEV_ADDR_00) /**< I2C address of the EEPROM device */
#endif
#ifndef AT24CXXX_PARAM_PIN_WP
#define AT24CXXX_PARAM_PIN_WP          (GPIO_UNDEF) /**< EEPROM write protect pin */
#endif
#ifndef AT24CXXX_PARAM_EEPROM_SIZE
    #if defined (MODULE_AT24C256) || defined (MODULE_AT24C256C)
    #define AT24CXXX_PARAM_EEPROM_SIZE     (AT24C256_EEPROM_SIZE) /**< EEPROM memory capacity in bytes */
    #elif defined (MODULE_AT24C128)
    #define AT24CXXX_PARAM_EEPROM_SIZE     (AT24C128_EEPROM_SIZE)
    #else
    #pragma message( \
        "[AT24CXXX] Module name was neither at24c256 nor at24c256c nor at24c128")
    #pragma message( \
        "[AT24CXXX] Using AT24C128_EEPROM_SIZE as a default parameter")
    #define AT24CXXX_PARAM_EEPROM_SIZE     (AT24C128_EEPROM_SIZE)
    #endif
#endif
#ifndef AT24CXXX_PARAMS
#define AT24CXXX_PARAMS         {              \
        .i2c = AT24CXXX_PARAM_I2C,        \
        .dev_addr = AT24CXXX_PARAM_ADDR,       \
        .pin_wp = AT24CXXX_PARAM_ADDR,       \
        .eeprom_size = AT24CXXX_PARAM_EEPROM_SIZE \
}   /**< Default device configuration parameters */
#endif
/** @} */

/**
 * @brief Number of configured AT24CXXX EEPROM devices
 */
#define AT24CXXX_NUMOF ARRAY_SIZE(at24cxxx_params)

/**
 * @brief   AT24CXXX configuration
 */
static const at24cxxx_params_t at24cxxx_params[] =
{
    AT24CXXX_PARAMS
};

#ifdef __cplusplus
}
#endif

#endif /* AT24CXXX_PARAMS_H */
/** @} */
