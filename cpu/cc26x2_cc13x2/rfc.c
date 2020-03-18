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
 * @}
 */

#include <driverlib/rfc.h>

void rfc_clock_enable(void)
{
    /* TODO: this is a temporary solution, this should be removed along with
     * driverlib in order to use RIOT definitions (which is a constant WIP).
     */
    RFCClockEnable();
}
