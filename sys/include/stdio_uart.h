/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio_uart STDIO over UART
 * @ingroup     sys
 *
 * @brief       Standard input/output backend using UART
 *
 * @warning Standard input is disabled by default on UART. To enable it, load
 *          the `stdin` module in your application:
 * ```
 * USEMODULE += stdin
 * ```
 *
 * @attention   Using STDIO over UART from interrupt context is not officially
 *              supported
 *
 * For testing purposes and using STDIO within an ISR should mostly work good
 * enough and for some platforms even reliable. Production code however should
 * fully avoid any access to STDIO from interrupt context. Instead, e.g. an
 * event could be posted to an @ref sys_event and the actual STDIO operation
 * being deferred to thread context.
 *
 * Some reasons why RIOT does not officially support STDIO over UART from ISR:
 * 1. UART is *slow* and the system easily remains in interrupt context for
 *    unacceptable long time.
 * 2. An UART implementation might choose to not guard an atomic sequence of
 *    interactions with the hardware by disabling IRQs but using a mutex, e.g.
 *    because the sequence takes long. Locking a mutex from interrupt
 *    context will result in a deadlock, if the mutex is already locked.
 * 3. If an ISR is triggered from a power saving mode, some peripherals or
 *    clock domains might still be offline during that ISR; including the UART
 *    peripheral. This is a valid implementation choice to allow time critical
 *    low power scenarios being covered by RIOT.
 *
 * @{
 * @file
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef STDIO_UART_H
#define STDIO_UART_H

/* Boards may override the default STDIO UART device */
#include "board.h"
#include "stdio_base.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef STDIO_UART_DEV
/**
 * @brief UART device to use for STDIO
 */
#define STDIO_UART_DEV          UART_DEV(0)
#endif

#ifndef STDIO_UART_BAUDRATE
/**
 * @brief Baudrate for STDIO
 */
#define STDIO_UART_BAUDRATE     (115200)
#endif

#ifndef STDIO_UART_RX_BUFSIZE
/**
 * @brief Buffer size for STDIO
 */
#define STDIO_UART_RX_BUFSIZE   (64)
#endif

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_UART_H */
