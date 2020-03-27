/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * @file
 * @brief       RIOT adaption of the "board" bsp module
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Tengfei Chang <tengfei.chang@gmail.com>, July 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2017
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "sctimer.h"
#include "radio.h"

#include "thread.h"
#include "periph/pm.h"

#include "openwsn_board.h"
#include "openwsn_debugpins.h"
#include "openwsn_debugpins_params.h"
#include "openwsn_leds.h"
#include "openwsn_leds_params.h"
#include "openwsn_uart.h"

#define LOG_LEVEL LOG_NONE
#include "log.h"

void board_init_openwsn(void)
{
    LOG_DEBUG("[openwsn/board]: init\n");

    if (IS_USED(MODULE_OPENWSN_LEDPINS)) {
        LOG_DEBUG("[openwsn/board]: leds init\n");
        ledpins_riot_init(openwsn_ledpins_params);
    }

    if (IS_USED(MODULE_OPENWSN_DEBUGPINS)) {
        LOG_DEBUG("[openwsn/board]: leds init\n");
        openwsn_debugpins_init(openwsn_debugpins_params);
    }

    if (IS_USED(MODULE_OPENWSN_SCTIMER)) {
        LOG_DEBUG("[openwsn/board]: sctimer init\n");
        sctimer_init();
    }

    if (IS_USED(MODULE_OPENWSN_SERIAL)) {
        LOG_DEBUG("[openwsn/board]: uart init\n");
        uart_init_openwsn();
    }
}

void board_sleep(void)
{
    /* sleep is handled by `pm_layered` */
}

void board_reset(void)
{
    LOG_DEBUG("[openwsn/board]: reset\n");
    pm_reboot();
}
