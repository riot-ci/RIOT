/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @brief       PWM extension for the PCA9685 I2C PWM controller
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @file
 */

#ifndef PWM_EXT_CONF_H
#define PWM_EXT_CONF_H

#if MODULE_EXTEND_PWM

#include <stddef.h>

#include "extend/pwm.h"
#include "pca9685_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Reference to PCA9685 device driver struct
 */
extern const pwm_ext_driver_t pca9685_extend_pwm_driver;
/** @} */

/**
 * @brief   References to the PCA9685 devices
 */
extern pca9685_t pca9685_dev[];

/**
 * @brief   PWM extension list of PCA9685 devices
 */
static const pwm_ext_t pwm_ext_list[] =
{
    {
        .driver = &pca9685_extend_pwm_driver,
        .dev = (void *)&pca9685_dev[0],
    },
};

#ifdef __cplusplus
}
#endif

#endif /* MODULE_EXTEND_PWM */

#endif /* PWM_EXT_CONF_H */
/** @} */
