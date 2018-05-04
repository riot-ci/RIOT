/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       low power mode test application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "led.h"
#include "pm_layered.h"

#include "periph/gpio.h"

/*
void gpio_reset(void)
{
    periph_clk_en(AHB1, 0x1f);

    for (unsigned base = GPIOA_BASE; base < GPIOE_BASE; base += 0x00000400) {
        GPIO_TypeDef *port = (GPIO_TypeDef*)base;
        port->MODER = 0xFFFFFFFF;
    }

    GPIO_TypeDef *port = (GPIO_TypeDef*)GPIOH_BASE;
    port->MODER = 0xFFFFFFFF;
    periph_clk_dis(AHB1, 0x1f);
}
*/
int main(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_init(BTN0_PIN, GPIO_IN_PD);

    LED0_ON;
    while (gpio_read(BTN0_PIN));
    LED0_OFF;

//    gpio_reset();

    while(1) {
       pm_set(1);
    }

    return 0;
}
