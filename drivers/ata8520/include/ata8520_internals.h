/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ata8520
 * @brief       Internal addresses, registers, constants for the ATA8520 device
 * @{
 * @file
 * @brief       Internal addresses, registers, constants for the ATA8520 device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef ATA8520_INTERNALS_H
#define ATA8520_INTERNALS_H

/**
 * SPI commands for ATA8520
 */
#define ATA8520_SYSTEM_RESET                (0x01)
#define ATA8520_IO_INIT                     (0x02)
#define ATA8520_IO_WRITE                    (0x03)
#define ATA8520_IO_READ                     (0x04)
#define ATA8520_OFF_MODE                    (0x05)
#define ATA8520_ATMEL_VERSION               (0x06)
#define ATA8520_WRITE_TX_BUFFER             (0x07)
#define ATA8520_TEST_MODE                   (0x08)
#define ATA8520_SIGFOX_VERSION              (0x09)
#define ATA8520_GET_STATUS                  (0x0A)
#define ATA8520_SEND_BIT                    (0x0B)
#define ATA8520_SEND_FRAME                  (0x0D)
#define ATA8520_GET_PAC                     (0x0F)
#define ATA8520_WRITE_SYS_CONF              (0x11)
#define ATA8520_GET_ID                      (0x12)
#define ATA8520_READ_SUP_TEMP               (0x13)
#define ATA8520_START_MEASUREMENT           (0x14)
#define ATA8520_TX_TEST_MODE                (0x15)
#define ATA8520_SEND_CW                     (0x17)
#define ATA8520_SET_TX_FREQUENCY            (0x1B)

/**
 * Constants for ATA8520
 */
#define ATA8520_ATMEL_SYSTEM_READY_MASK     (0x20)
#define ATA8520_ATMEL_FRAME_SENT_MASK       (0x10)
#define ATA8520_ATMEL_PA_MASK               (0x01)

#endif /* ATA8520_INTERNALS_H */
