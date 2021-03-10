/*
 * Copyright (C) 2019 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_bastwan
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the bastwan board
 *
 *
 * @author      h-filzer
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

#ifdef MODULE_SX127X
#include "sx127x_params.h"
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LED */
    gpio_init(LED0_PIN, GPIO_OUT);

#ifdef TX_SWITCH_PWR_PIN
    gpio_init(TX_SWITCH_PWR_PIN, GPIO_OUT);
    gpio_set(TX_SWITCH_PWR_PIN);
#endif /* TX_SWITCH_PWR_PIN */
       /* initialize board specific pins for LoRa */
#ifdef MODULE_SX127X
    gpio_init(TCXO_PWR_PIN, GPIO_OUT);
    gpio_set(TCXO_PWR_PIN);
    gpio_init(TX_OUTPUT_SEL_PIN, GPIO_OUT);
    gpio_write(TX_OUTPUT_SEL_PIN, !SX127X_PARAM_PASELECT);
#endif /* USEMODULE_SX127X */
}
