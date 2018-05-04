/*
 * Copyright (C) 2018 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sens0
 * @brief       Sens0 SAUL driver helpers
 *
 * These functions are helper functions for integrating Sens0 with SAUL.
 *
 * Sens0 spec writers: see usage examples in drivers/sens0/specs
 *
 * @{
 *
 * @file
 * @brief       Sens0 SAUL driver helpers
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef SENS0_SAUL_H
#define SENS0_SAUL_H

#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Read value from the first output channel on the sensor
 *
 * This is equivalent to @ref sens0_i2c_read with the @c iout argument bound to 0
 *
 * Useful mainly in SAUL driver definitions.
 */
int sens0_i2c_read_output0(const void *dev, phydat_t *res);

/**
 * @brief   Read value from the second output channel on the sensor
 *
 * This is equivalent to @ref sens0_i2c_read with the @c iout argument bound to 1
 *
 * Useful mainly in SAUL driver definitions.
 */
int sens0_i2c_read_output1(const void *dev, phydat_t *res);

/**
 * @brief   Read value from the third output channel on the sensor
 *
 * This is equivalent to @ref sens0_i2c_read with the @c iout argument bound to 2
 *
 * Useful mainly in SAUL driver definitions.
 */
int sens0_i2c_read_output2(const void *dev, phydat_t *res);

#ifdef __cplusplus
}
#endif

#endif /* SENS0_H */
/** @} */
