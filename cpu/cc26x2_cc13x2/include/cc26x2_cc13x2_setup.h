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
 * @brief           CC26x2/CC13x2 API to setup MCU after reset/wakeup/powerdown.
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef CC26X2_CC13X2_SETUP_H
#define CC26X2_CC13X2_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Performs the necessary trim of the device which is not done in ROM
 *          boot code.
 *
 * This function should only execute coming from ROM boot.
 *
 * The following is handled by this function:
 *
 *  - Checks if the driverlib variant used by the application is supported by
 *  the device. Execution is halted in case of unsupported driverlib variant.
 *  - Configures VIMS cache mode based on setting in CCFG.
 *  - Configures functionalities like DCDC and XOSC dependent on startup modes
 *  like cold reset, wakeup from shutdown and wakeup from from powerdown.
 *  - Configures VIMS power domain control.
 *  - Configures optimal wait time for flash FSM in cases where flash pump
 *  wakes up from sleep.
 */
void setup_trim_device(void);

#ifdef __cplusplus
}
#endif

#endif /* CC26X2_CC13X2_SETUP_H */

/*@}*/
