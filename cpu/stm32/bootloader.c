/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Trigger reset to the bootloader stored in the internal boot ROM
 *              memory.
 *
 *              This will start the DFU bootloader on the USB interface.
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"

#define BOOTLOADER_MAGIC    0xB007AFFE

static uint32_t _magic __attribute__((section(".noinit")));

void pre_startup(void)
{
    if (_magic != BOOTLOADER_MAGIC) {
        return;
    }

    /* clear magic */
    _magic = 0;

    /* enable SYSCFG clock */
    RCC->APB2ENR   = RCC_APB2ENR_SYSCFGEN;

    /* remap ROM at zero */
    SYSCFG->MEMRMP = SYSCFG_MEMRMP_MEM_MODE_0;

    /* jump to the bootloader */
    __asm__ volatile("ldr r0, =%0" :: "i" (STM32_LOADER_ADDR));
    __asm__ volatile("ldr sp, [r0]");
    __asm__ volatile("ldr pc, [r0, #4]");
}

void __attribute__((weak)) usb_board_reset_in_bootloader(void)
{
    _magic = BOOTLOADER_MAGIC;
    NVIC_SystemReset();
}
