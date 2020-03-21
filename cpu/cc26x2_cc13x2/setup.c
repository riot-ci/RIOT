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
 * @}
 */

#include <driverlib/setup.h>

void setup_trim_device(void)
{
    /* TODO: rewrite this without using driverlib, this can be a general
     * implementation for the `cc26x0` and the `cc26x2_cc13x2` MCUs.
     *
     * This function has some undocumented edge cases inside and hence the
     * complexity to port it. Also there are missing some register definitions.
     *
     * This function is isolated in this file so the driverlib definitions
     * don't collide with RIOT ones, however, this file can be kept for the
     * rewrite.
     */
    SetupTrimDevice();
}
