/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Board initialization for the nRF52xxx DK
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"

#if defined(BOARD_NRF52DK)
#define RESET_PIN       (21U)
#elif defined(BOARD_NRF52840DK)
#define RESET_PIN       (18U)
#endif

void board_init(void)
{
    /* For nRF52x CPUs, the reset pin is programmable. The reset pin
     * configuration is store in two persistent registers, that are programmed
     * in the same way than the CPUs flash memory.
     *
     * In most cases, this values should be readily programmed and thus do not
     * need to be touched. We have seen however a number of nrf52xxxdk boards,
     * where this was not the case, hence the code block below.
     *
     *  NOTE: the board has to be reset once after the new reset pin was
     *        programmed for the changes to take effect */
#ifdef RESET_PIN
    if (NRF_UICR->PSELRESET[0] != RESET_PIN) {
        /* NOTE: The PSELRESET registers behave in the same way as flash memory,
         *       where only 1s can be written to 0s. Once a bit is set to 0, it
         *       can only be reverted to 1 by erasing the content of ALL UICR
         *       registers. So that is really needed, comment out the block
         *       below... */
        /*
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Een;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
        NRF_NVMC->ERASEUICR = 1;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
        */
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Wen;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
        NRF_UICR->PSELRESET[0] = RESET_PIN;
        NRF_UICR->PSELRESET[1] = RESET_PIN;
        while (NRF_NVMC->READY == NVMC_READY_READY_Busy) {}
        NRF_NVMC->CONFIG = NVMC_CONFIG_WEN_Ren;
    }
#endif

    /* initialize the boards LEDs */
    LED_PORT->DIRSET = (LED_MASK);
    LED_PORT->OUTSET = (LED_MASK);

    /* initialize the CPU */
    cpu_init();
}
