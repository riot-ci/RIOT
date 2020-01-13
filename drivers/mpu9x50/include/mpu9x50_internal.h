/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mpu9x50
 *
 * @{
 * @file
 * @brief       Internal config for the MPU-9X50 (MPU9150 and MPU9250)
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */
#ifndef MPU9X50_INTERNAL_H
#define MPU9X50_INTERNAL_H

 #ifdef __cplusplus
 extern "C" {
 #endif

/**
 * @name    MPU-9X50 temperature calibration values
 * @{
 */
#ifdef MODULE_MPU9150
#define MPU9X50_TEMP_SENSITIVITY    340
#define MPU9X50_TEMP_OFFSET         35
#elif defined(MODULE_MPU9250)
#define MPU9X50_TEMP_SENSITIVITY    333.87
#define MPU9X50_TEMP_OFFSET         21
#else
#error "MPU9X50 DRIVER not selected or supported"
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* MPU9X50_INTERNAL_H */
/** @} */
