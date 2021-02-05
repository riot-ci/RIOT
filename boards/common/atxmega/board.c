/*
 * Copyright (C) 2018 RWTH Aachen, Josua Arndt <jarndt@ias.rwth-aachen.de>
 *               2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common
 * @{
 *
 * @file
 * @brief       Common implementations for ATxmega boards
 *
 * @author      Josua Arndt <jarndt@ias.rwth-aachen.de>
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include "board.h"
#include "cpu.h"

void led_init(void);

void __attribute__((weak)) board_init(void)
{
    cpu_init();
#ifdef LED_PORT
    led_init();
#endif
}
