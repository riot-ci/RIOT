/*
 * Copyright (C) 2020 iosabi
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_qn908x
 * @{
 *
 * @file
 * @brief       QN908x CPU initialization
 *
 * @author      iosabi <iosabi@protonmail.com>
 * @}
 */

#include "cpu.h"
#include "periph/init.h"

#include "stdio_base.h"

#ifndef MODULE_PERIPH_WDT
#include "vendor/drivers/fsl_clock.h"
#endif /* ndef MODULE_PERIPH_WDT */

/**
 * @brief Initialize the CPU
 */
void cpu_init(void)
{
    /* initialize the Cortex-M core */
    cortexm_init();
#ifndef MODULE_PERIPH_WDT
    /* If the `periph_wdt` is *not* being used (because the user does not care
     * about that feature) we need to disable the Watchdog and continue running
     * without it. Otherwise the CPU will reboot after about 10 seconds.
     */
    CLOCK_DisableClock(kCLOCK_Wdt);
#endif /* ndef MODULE_PERIPH_WDT */

    /* TODO: It would be good to move the VTOR to SRAM to allow execution from
     * RAM with the FLASH memory off to allow for ultra low power operation on
     * sleep mode. This needs to be done after cortexm_init() since it sets the
     * VTOR to _isr_vectors which is the address on FLASH.
     */

    /* initialize stdio prior to periph_init() to allow use of DEBUG() there */
    stdio_init();
    /* trigger static peripheral initialization */
    periph_init();
}
