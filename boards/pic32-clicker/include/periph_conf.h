/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @defgroup    boards_pic32-clicker MikroE PIC32 Clicker
 * @ingroup     boards
 * @brief       peripheral configuration for the MikroE PIC32 Clicker
 * @{
 *
 * @file
 * @brief       peripheral configuration for the MikroE PIC32 Clicker
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief   The peripheral clock is required for the UART Baud rate calculation
 *          It is configured by the 'config' registers (see pic32_config_settings.c)
 *          Note 120MHz is the max F for this device.
 */
#define PERIPHERAL_CLOCK (96000000)  /* Hz */

/**
 * @name    Timer definitions
 * @{
 */
#define TIMER_NUMOF         (1)
#define TIMER_0_CHANNELS    (3)
/** @} */

/**
  * @name    UART Definitions
  *          There are 4 UARTS available on this CPU.
  *          We route debug via UART3 on this board,
  *          this is the UART connected to the MikroBUS
  *
  *          Note Microchip number the UARTS 1->4
  * @{
  */
#define UART_NUMOF          (4)
#ifndef MODULE_NEWLIB_SYSCALLS_MIPS_UHI
#define DEBUG_VIA_UART      (3)
#define DEBUG_UART_BAUD     (9600)
#endif /* !defined(MODULE_NEWLIB_SYSCALLS_MIPS_UHI) */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
