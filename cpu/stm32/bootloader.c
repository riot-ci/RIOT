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
 *              This will start the DFU/UART/SPI bootloader.
 *              See application note AN2606 for which options are available on
 *              your individual MCU.
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
#if defined(RCC_APB2ENR_SYSCFGEN)
    RCC->APB2ENR   = RCC_APB2ENR_SYSCFGEN;
#elif defined(RCC_APB2ENR_SYSCFGCOMPEN)
    RCC->APB2ENR   = RCC_APB2ENR_SYSCFGCOMPEN
#endif

    /* remap ROM at zero */
#if defined(SYSCFG_MEMRMP_MEM_MODE_0)
    SYSCFG->MEMRMP = SYSCFG_MEMRMP_MEM_MODE_0;
#elif defined(SYSCFG_CFGR1_MEM_MODE_0)
    SYSCFG->CFGR1  = SYSCFG_CFGR1_MEM_MODE_0;
#endif

    /* jump to the bootloader */
    __asm__ volatile("ldr r0, =%0" :: "i" (STM32_LOADER_ADDR));
    __asm__ volatile("mov sp, r0");
    __asm__ volatile("mov pc, r0");
}

void __attribute__((weak)) usb_board_reset_in_bootloader(void)
{
    _magic = BOOTLOADER_MAGIC;
    NVIC_SystemReset();
}
