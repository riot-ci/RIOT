/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_stm32
 * @{
 *
 * @file
 * @brief       Common configuration for STM32 OTG FS peripheral
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef CFG_USB_OTG_FS_H
#define CFG_USB_OTG_FS_H

#include "periph_cpu.h"

#define STM32_USB_OTG_FS_ENABLED

/**
 * @name common USB OTG FS configuration
 * @{
 */
static const stm32_fshs_usb_config_t stm32_fshs_usb_config[] = {
    {
        .type     = STM32_USB_OTG_FS,
        .phy      = STM32_USB_OTG_PHY_BUILTIN,
        .dm       = GPIO_PIN(PORT_A, 11),
        .dp       = GPIO_PIN(PORT_A, 12),
        .af       = GPIO_AF10,
        .rcc_mask = RCC_AHB2ENR_OTGFSEN,
        .ahb      = AHB2,
        .periph   = (uint8_t*)USB_OTG_FS_PERIPH_BASE,
        .irqn     = OTG_FS_IRQn,
    }
};

#define USBDEV_NUMOF           ARRAY_SIZE(stm32_fshs_usb_config)

#ifdef __cplusplus
}
#endif

#endif /* CFG_USB_OTG_FS_H */
/** @} */
