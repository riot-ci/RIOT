/*
 * Copyright (C) 2018 Hamburg University of Applied Sciences
 *               2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_openwsn
 * @{
 *
 * In RIOT, the first configured UART device is mapped to STDIO in most cases.
 * In OpenWSN however, the `openserial` tool uses UART to feed external software
 * running on a host computer such as
 * [Openvisualizer](https://github.com/openwsn-berkeley/openvisualizer).
 * To enable use of these tools, we provide a UART adaptation.
 *
 * By default when `openwsn_serial` (`openserial`) is used STDIO will be
 * disabled (it will use `stdio_null`). When multiple uart are available STDIO
 * and `openserial` can be used in parallel..
 *
 * OpenWSN uart abstraction makes use of tx hardware interrupts to execute a
 * previously registered callback after every byte is sent out. This interrupts
 * are currently not defined in RIOT in a generic way, so instead we set a timer
 * which fires shortly after a byte was written.
 *
 * This implementation uses `ztimer` since its already used as an abstraction
 * for `periph_rtt`.
 *
 * @file
 * @brief       RIOT adaption-specific definition of the "uart" bsp module.
 *
 * @author      Thomas Watteyne <watteyne@eecs.berkeley.edu>, February 2012
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>, July 2018
 *
 * @}
 */

#ifndef OPENWSN_UART_H
#define OPENWSN_UART_H

#include "stdint.h"
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   OpenWSN default uart dev.
 *
 * @note    If only one UART is available that uart might collide with
 *          STDIO_UART_DEV, otherwise the next available UART will be used.
 *
 */
#ifndef STDIO_UART_DEV
#define STDIO_UART_DEV              (UART_DEV(0))
#endif
#ifndef OPENWSN_UART_DEV
#ifdef MODULE_STDIO_NULL
#define OPENWSN_UART_DEV            (STDIO_UART_DEV)
#else
#define OPENWSN_UART_DEV            ((STDIO_UART_DEV + 1) % UART_NUMOF)
#endif
#endif

/**
 * @brief   OpenWSN uart baudrate
 */
#ifndef STDIO_UART_BAUDRATE
#define OPENWSN_UART_BAUDRATE       (115200U)
#else
#define OPENWSN_UART_BAUDRATE       (STDIO_UART_BAUDRATE)
#endif

/**
 * @brief   OpenWSN uart tx_isrt callback offset in ticks
 */
#ifndef OPENWSN_UART_TX_CB_OFFSET
#define OPENWSN_UART_TX_CB_OFFSET   (10U)
#endif

/**
 * @brief   Initialize OpenWSN uart
 *
 * This will initialize a uart device at STDIO_UART_BAUDRATE/8N1. It will also
 * initialize a timer to set sw tx_isr.
 *
 */
void    uart_init_openwsn(void);

/**
 * @brief   OpenWSN uart tx callback type
 */
typedef void (*uart_tx_cbt)(void);
/**
 * @brief   OpenWSN uart rx callback type
 */
typedef void (*uart_rx_cbt)(void);

/**
 * @brief   OpenWSN uart tx callback type
*/
void    uart_setCallbacks(uart_tx_cbt txCb, uart_rx_cbt rxCb);

/**
 * @brief   Sets software flow control CTS
 *
 * This function sends XON or XOFF bytes to "set" or "unset" CTS by sw.
 *
 * @param[in] state         true sets CTS, false clears CTS
 */
void    uart_setCTS(bool state);

/**
 * @brief   Write a single byte to the configured OpenWSN uart
 */
void    uart_writeByte(uint8_t byteToWrite);

/**
 * @brief   Reads a single byte received through uart.
 *
 * OpenWSN will call this thrpugh uart_rx_cbt.
 *
 */
uint8_t uart_readByte(void);

/**
 * @brief   Function definitions that we do not use in RIOT but that need to be
 *          defined for OpenWSN
 */
void    uart_enableInterrupts(void);
void    uart_disableInterrupts(void);
void    uart_clearRxInterrupts(void);
void    uart_clearTxInterrupts(void);

#ifdef __cplusplus
}
#endif

#endif /* OPENWSN_UART_H */
