/*
 * Copyright (C) 2019 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_cryptoauthlib cryptoauthlib security crypto
 * @{
 *
 * @file
 * @brief       Default addresses and device descriptor for CryptoAuth devices
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#ifndef ATCA_H
#define ATCA_H

#include "periph/i2c.h"
#include "cryptoauthlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Default device and word addresses for ATECC508A
 */
#define ATCA_I2C_ADR (0xC0)    /**< Default device address is 0xC0 */

#define ATCA_SLEEP_ADR  (0x01)  /**< Word address to write to for sleep mode */
#define ATCA_IDLE_ADR   (0x02)  /**< Word address to write to for idle mode */
#define ATCA_DATA_ADR   (0x03)  /**< Word address to read and write to data area */

/**
 * @brief   Device descriptor contains ATCAIfaceCfg structure
 */
typedef struct {
    ATCAIfaceCfg params;        /**< Device configuration */
} atca_t;

#ifdef __cplusplus
}
#endif

#endif /* ATCA_H */
/** @} */
