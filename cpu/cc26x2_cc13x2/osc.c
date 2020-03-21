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
 * @brief           CC26x2/CC13x2 Oscillator functions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include <driverlib/osc.h>

/* TODO: rewrite this module using RIOT definitions. This code is isolated in
 * this file so driverlib definitions don't collide with RIOT ones.
 *
 * A general implementation for `cc26x0` and `cc26x2_cc13x2` MCUs can be made.
 * Also these functions exist in the MCU ROM.
 */

uint32_t osc_clock_source_get(uint32_t src_clk)
{
    return OSCClockSourceGet(src_clk);
}

void osc_clock_source_set(uint32_t src_clk, uint32_t osc)
{
    OSCClockSourceSet(src_clk, osc);
}

bool osc_hf_source_ready(void)
{
    return OSCHfSourceReady();
}

void osc_hf_source_switch(void)
{
    OSCHfSourceSwitch();
}
