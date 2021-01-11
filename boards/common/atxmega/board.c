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
#include "cpu_clock.h"
#include "cpu.h"
#include "irq.h"
#include "periph/gpio.h"
#include "periph/init.h"

#ifndef CPU_ATXMEGA_CLK_SCALE_INIT
#define CPU_ATXMEGA_CLK_SCALE_INIT    CPU_ATXMEGA_CLK_SCALE_DIV1
#endif
#ifndef CPU_ATXMEGA_BUS_SCALE_INIT
#define CPU_ATXMEGA_BUS_SCALE_INIT    CPU_ATXMEGA_BUS_SCALE_DIV1_1
#endif

void led_init(void);

void clk_init(void)
{
    uint8_t *reg = (uint8_t *)&PR.PRGEN;
    uint8_t i;

    /* Turn off all peripheral clocks that can be turned off. */
    for (i = 0; i <= 7; i++) {
        *(reg++) = 0xff;
    }

    reg = (uint8_t *)&PR.PRGEN;
    /* Turn on all peripheral clocks that can be turned on. */
    for (i = 0; i <= 7; i++) {
        *(reg++) = 0x00;
    }

    /* XMEGA A3U [DATASHEET] p.23 After reset, the device starts up running
     * from the 2MHz internal oscillator. The other clock sources, DFLLs
     * and PLL, are turned off by default.
     *
     * Configure clock to 32MHz with calibration
     * application note AVR1003
     *
     * From errata http://www.avrfreaks.net/forum/xmega-dfll-does-it-work
     * In order to use the automatic runtime calibration for the 2 MHz or
     * the 32 MHz internal oscillators, the DFLL for both oscillators and
     * both oscillators has to be enabled for one to work.
     */
    OSC.PLLCTRL = 0;

    /* Enable the internal PLL & 32MHz & 32KHz oscillators */
    OSC.CTRL |= OSC_PLLEN_bm | OSC_RC32MEN_bm | OSC_RC32KEN_bm;

    /* Wait for 32Khz oscillator to stabilize */
    while (!(OSC.STATUS & OSC_RC32KRDY_bm)) {}

    /* Wait for 32MHz oscillator to stabilize */
    while (!(OSC.STATUS & OSC_RC32MRDY_bm)) {}

    /* Enable DFLL - defaults to calibrate against internal 32Khz clock */
    DFLLRC32M.CTRL = DFLL_ENABLE_bm;

    /* Enable DFLL - defaults to calibrate against internal 32Khz clock */
    DFLLRC2M.CTRL = DFLL_ENABLE_bm;

    atxmega_set_prescaler(CPU_ATXMEGA_CLK_SCALE_INIT,
                          CPU_ATXMEGA_BUS_SCALE_INIT);

    /* Disable CCP for Protected IO register and set new value*/
    /* Switch to 32MHz clock */
    _PROTECTED_WRITE(CLK.CTRL, CLK_SCLKSEL_RC32M_gc);

    /*
     * Previous instruction takes 3 clk cycles with -Os option
     * we need another clk cycle before we can reuse it.
     */
    __asm__ __volatile__ ("nop");
}

void __attribute__((weak)) board_init(void)
{
    clk_init();

    cpu_init();
#ifdef LED_PORT
    led_init();
#endif
    irq_enable();
}
