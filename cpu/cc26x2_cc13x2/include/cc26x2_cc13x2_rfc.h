/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc26x2_cc13x2
 * @{
 *
 * @file
 * @brief           CC26x2/CC13x2 RF Core common functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_RFC_H
#define CC26X2_CC13X2_RFC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <assert.h>

/**
 * @brief   Turns on the radio core.
 *
 *          Sets up the power and resources for the radio core.
 *          - Switches the high frequency clock to the xosc crystal on
 *            CC26X2/CC13X2.
 *          - Initializes the rx buffers and command
 *          - Powers on the radio core power domain
 *          - Enables the radio core power domain
 *          - Sets up the interrupts
 */
void cc26x2_cc13x2_rf_power_on(void);

/**
 * @brief   Turns off the radio core.
 *
 *          Switches off the power and resources for the radio core.
 *          - Disables the interrupts
 *          - Disables the radio core power domain
 *          - Powers off the radio core power domain
 *          - On CC13X2/CC26X2 switches the high frequency clock to the rcosc
 *            to save power
 */
void cc26x2_cc13x2_rf_power_off(void);

/**
 * @brief   Enable the Versatile Memory Instruction System
 *
 *          This needs to be called before doing any RF operation.
 */
void cc26x2_cc13x2_enable_vims(void);

#ifdef __cplusplus
}
#endif

#endif /* CC26X2_CC13X2_RFC_H */

/*@}*/
