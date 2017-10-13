/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_stm32_common
 * @{
 *
 * @file
 * @brief           Memory definitions for the STM32 family
 *
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 *
 * @}
 */

boot_offset = DEFINED( offset ) ? offset : 0x0 ;
ccmram_length = DEFINED( ccmram_len ) ? ccmram_len : 0x0 ;

MEMORY
{
    rom (rx)    : ORIGIN = 0x08000000 + boot_offset, LENGTH = rom_length
    ram (rwx)   : ORIGIN = 0x20000000,               LENGTH = ram_length
    ccmram (rwx): ORIGIN = 0x10000000,               LENGTH = ccmram_length
}

INCLUDE cortexm_base.ld
