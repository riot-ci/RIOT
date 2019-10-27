/*
 * Copyright (C) 2016 Leon George
 * Copyright (C) 2018 Anton Gerasimov
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cc13x2
 * @{
 *
 * @file
 * @brief       implementation of the CPU initialization
 *
 * @author      Leon M. George <leon@georgemail.eu>, Anton Gerasimov <tossel@gmail.com>
 * @}
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/init.h"
#include "stdio_base.h"

#include "cc13x2_prcm.h"

#ifndef HF_CLOCK_SOURCE
#define HF_CLOCK_SOURCE DDI_0_OSC_CTL0_SCLK_HF_SRC_SEL_RCOSC /* set 48MHz RCOSC */
#endif
#ifndef LF_CLOCK_SOURCE
#define LF_CLOCK_SOURCE DDI_0_OSC_CTL0_SCLK_LF_SRC_SEL_HF_RCOSC /* set 31.25kHz derived from 48MHz RCOSC */
#endif

/**
 *@brief Configure the MCU system clock
 */
static void cpu_clock_init(void);

/**
 * @brief Initialize the CPU, set IRQ priorities
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();

    /* initialize the system clock */
    cpu_clock_init();

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();

    /* trigger static peripheral initialization */
    periph_init();
}

static void cpu_clock_init(void)
{
    /* nothing to be done for cc13x2/cc26x2 */
}
