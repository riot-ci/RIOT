/*
 * Copyright (C) 2021  Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_nucleo-wl55jc
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the Nucleo-wl55jc board
 *
 *
 * @author      Akshai M <akshai.m@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "periph/gpio.h"

#include "sx126x.h"

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    if (IS_USED(MODULE_SX126X_STM32WL)) {
        /* Initialize the GPIO control for RF 3-port switch (SP3T) */
        gpio_init(FE_CTRL1, GPIO_OUT);
        gpio_init(FE_CTRL2, GPIO_OUT);
        gpio_init(FE_CTRL3, GPIO_OUT);
    }

    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(LED1_PIN, GPIO_OUT);
    gpio_init(LED2_PIN, GPIO_OUT);

    if (IS_ACTIVE(CONFIG_WL55JC_SUBGHZ_DEBUG)) {
        /* SUBGHZSPI Debug */
        gpio_init(GPIO_PIN(PORT_A, 7), GPIO_OUT); /* DEBUG_SUBGHZSPI_MOSIOUT */
        gpio_init_af(GPIO_PIN(PORT_A, 7), GPIO_AF13);
        gpio_init(GPIO_PIN(PORT_A, 6), GPIO_OUT); /* DEBUG_SUBGHZSPI_MISOOUT */
        gpio_init_af(GPIO_PIN(PORT_A, 6), GPIO_AF13);
        gpio_init(GPIO_PIN(PORT_A, 5), GPIO_OUT); /* DEBUG_SUBGHZSPI_SCKOUT */
        gpio_init_af(GPIO_PIN(PORT_A, 5), GPIO_AF13);
        gpio_init(GPIO_PIN(PORT_A, 4), GPIO_OUT); /* DEBUG_SUBGHZSPI_NSSOUT */
        gpio_init_af(GPIO_PIN(PORT_A, 4), GPIO_AF13);

        /* Sub-GHz Radio Debug */
        gpio_init(GPIO_PIN(PORT_A, 12), GPIO_OUT); /* RF_BUSY */
        gpio_init_af(GPIO_PIN(PORT_A, 12), GPIO_AF6);
        gpio_init(GPIO_PIN(PORT_A, 11), GPIO_OUT); /* DEBUG_RF_NRESET */
        gpio_init_af(GPIO_PIN(PORT_A, 11), GPIO_AF13);
        gpio_init(GPIO_PIN(PORT_B, 2), GPIO_OUT); /* DEBUG_RF_SMPSRDY */
        gpio_init_af(GPIO_PIN(PORT_B, 2), GPIO_AF13);
        gpio_init(GPIO_PIN(PORT_B, 4), GPIO_OUT); /* DEBUG_RF_LDORDY */
        gpio_init_af(GPIO_PIN(PORT_B, 4), GPIO_AF13);
        gpio_init(GPIO_PIN(PORT_A, 10), GPIO_OUT); /* DEBUG_RF_HSE32RDY */
        gpio_init_af(GPIO_PIN(PORT_A, 10), GPIO_AF13);
    }
}

/**
 * @brief Callback to set RF switch mode
 *
 * This function sets the GPIO's wired to the SP3T RF Switch. Nucleo-WL55JC
 * supports three modes of operation.
 */
void nucleo_wl55jc_sx126x_set_rf_mode (sx126x_t *dev, sx126x_rf_mode_t rf_mode)
{
    (void) dev;
    switch (rf_mode) {
    case SX126X_RF_MODE_RX:
        gpio_set(FE_CTRL1);
        gpio_clear(FE_CTRL2);
        gpio_set(FE_CTRL3);
        break;

    case SX126X_RF_MODE_TX_LPA:
        gpio_set(FE_CTRL1);
        gpio_set(FE_CTRL2);
        gpio_set(FE_CTRL3);
        break;

    case SX126X_RF_MODE_TX_HPA:
        gpio_clear(FE_CTRL1);
        gpio_set(FE_CTRL2);
        gpio_set(FE_CTRL3);
        break;

    default:
        break;
    }
}
