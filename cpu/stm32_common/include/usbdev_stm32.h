/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    cpu_stm32_common_usbdev stm32 USB OTG FS/HS peripheral
 * @ingroup     cpu_stm32_common
 * @brief       USB interface functions for the stm32 class devices
 *
 * @{
 *
 * @file
 * @brief       USB interface functions for the stm32 OTG FS/HS class devices
 *
 * The stm32f2, stm32f4 and stm32f7 have a common USB OTG FS capable USB
 * peripheral.
 *
 * Two versions are currently known to exist with subtle differences
 * in some registers. The CID register of the peripheral indicates this version,
 * 0x00001200 for one version of the full speed peripheral and 0x00002000 for
 * the other version of the full speed peripheral.
 * The main difference is in the GCCFG register, where the 1.2 version has a
 * NOVBUSSENS bit and the 2.0 version has a VBDEN bit. This difference is used
 * to detect the IP version.
 * The 2.0 version also has more advanced USB low power mode support.
 *
 * For the end user, the main difference is the 1.2 version  having 4 endpoints
 * and the 2.0 version having 6 endpoints. The 2.0 version also supports a
 * number of USB low power modes.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef USBDEV_STM32_H
#define USBDEV_STM32_H

#include <stdint.h>
#include <stdlib.h>
#include "periph_cpu.h"
#include "periph/usbdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Detect the IP version based on the available register define */
#if defined(USB_OTG_GCCFG_NOVBUSSENS)
#define STM32_USB_OTG_CID_12     /**< USB OTG FS version 0x00001200 */
#elif defined(USB_OTG_GCCFG_VBDEN)
#define STM32_USB_OTG_CID_20     /**< USB OTG FS version 0x00002000 */
#else
#error Unknown USB peripheral version
#endif

/**
 * @brief Buffer space available for endpoint TX/RX data
 */
#ifndef STM32_USB_FS_BUF_SPACE
#define STM32_USB_FS_BUF_SPACE  USBDEV_EP_BUF_SPACE
#endif

/**
 * @brief Number of endpoints available with the OTG FS peripheral
 *        including the control endpoint
 */
#ifdef STM32_USB_OTG_CID_12
#define STM32_USB_FS_NUM_EP (4) /**< OTG FS with 4 endpoints */
#elif defined(STM32_USB_OTG_CID_20)
#define STM32_USB_FS_NUM_EP (6) /**< OTG FS with 6 endpoints */
#endif

/**
 * @brief Number of endpoints available with the OTG HS peripheral
 *        including the control endpoint
 */
#ifdef STM32_USB_OTG_CID_12
#define STM32_USB_HS_NUM_EP (6) /**< OTG HS with 6 endpoints */
#elif defined(STM32_USB_OTG_CID_20)
#define STM32_USB_HS_NUM_EP (9) /**< OTG HS with 9 endpoints */
#endif

/**
 * @brief USB OTG FS FIFO reception buffer space in 32-bit words
 */
#ifndef STM32_FS_OUT_FIFO_SIZE
#define STM32_FS_OUT_FIFO_SIZE 128
#endif

/**
 * @brief USB OTG HS FIFO reception buffer space in 32-bit words
 */
#ifndef STM32_HS_OUT_FIFO_SIZE
#define STM32_HS_OUT_FIFO_SIZE 512
#endif

/**
 * @brief Out endpoint struct for stm32 USB OTG peripherals
 */
typedef struct {
    usbdev_ep_t ep;     /**< Generic endpoint struct */
    size_t len;         /**< Bytes of data received on the last transfer */
} stm32_fshs_usb_ep_t;

/**
 * @brief stm32 USB OTG peripheral device context
 */
typedef struct {
    usbdev_t usbdev;                                /**< Inherited usbdev struct */
    const stm32_fshs_usb_config_t *config;          /**< USB peripheral config   */
    uint8_t buffer[STM32_USB_FS_BUF_SPACE];         /**< Buffer space for endpoints */
    size_t occupied;                                /**< Buffer space occupied */
    bool suspended;                                 /**< Suspend status */
    usbdev_ep_t *in;                                /**< In endpoints */
    stm32_fshs_usb_ep_t *out;                       /**< Out endpoints */
} stm32_fshs_usb_t;

#ifdef __cplusplus
}
#endif
#endif /* USBDEV_STM32_H */
/** @} */
