/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_qn9080dk
 * @{
 *
 * @file
 * @brief       Board specific implementations for the QN9080DK base board
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#include "periph/gpio.h"

#include "vendor/drivers/fsl_clock.h"

void clocks_init(void);

void board_init(void)
{
    /* Initialize the clock sources. */
    clocks_init();

    /* Initialize LEDs and Buttons. */
    gpio_init(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN, GPIO_OUT);
    gpio_init(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(BTN1_PIN, BTN1_MODE);
    gpio_init(BTN2_PIN, BTN2_MODE);

    /* initialize the CPU */
    cpu_init();
}

void clocks_init(void)
{
    /* Set up clock selectors - Attach clocks to the peripheries */

    /* Switch XTAL_CLK to 32M */
    CLOCK_AttachClk(k32M_to_XTAL_CLK);
    /* Switch 32K_CLK to XTAL32K */
    CLOCK_AttachClk(kXTAL32K_to_32K_CLK);
    /* Switch SYS_CLK to XTAL */
    CLOCK_AttachClk(kXTAL_to_SYS_CLK);
    /* Switch WDT_CLK to APB */
    CLOCK_AttachClk(kAPB_to_WDT_CLK);

    /* Set up dividers */

    /* Set OSC32M_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivOsc32mClk, 1U);
    /* Set XTAL_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivXtalClk, 1U);
    /* Set AHB_DIV divider to value 2 */
    CLOCK_SetClkDiv(kCLOCK_DivAhbClk, 1U);
    /* Set FRG_MULT1 to value 0, Set FRG_DIV1 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg1, 0U);
    /* Set FRG_MULT0 to value 0, Set FRG_DIV0 to value 255 */
    CLOCK_SetClkDiv(kCLOCK_DivFrg0, 0U);
    /* Set APB_DIV divider to value 1 */
    CLOCK_SetClkDiv(kCLOCK_DivApbClk, 0U);
}
