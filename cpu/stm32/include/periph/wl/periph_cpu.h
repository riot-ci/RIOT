/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           STM32WL CPU specific definitions for internal peripheral handling
 *
 * @author          Akshai M <akshai.m@fu-berlin.de>
 *
 */

#ifndef PERIPH_WL_PERIPH_CPU_H
#define PERIPH_WL_PERIPH_CPU_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606 ( Table 143 : System memory)
 */
#define STM32_BOOTLOADER_ADDR   (0x1FFF0000)

#endif /* ndef DOXYGEN */

/**
 * @defgroup        cpu_stm32_wl_debug STM32WL hardware debug pin definitions
 * @ingroup         cpu_stm32
 * @{
 */
/**
 * @defgroup        cpu_stm32_wl_debug_subghz_spi STM32WL Sub-GHz SPI debug pins
 * @ingroup         cpu_stm32_wl_debug
 * @{
 */
#define CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT     GPIO_PIN(PORT_A, 7)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_MOSIOUT_AF  GPIO_AF13

#define CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT     GPIO_PIN(PORT_A, 6)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_MISOOUT_AF  GPIO_AF13

#define CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT      GPIO_PIN(PORT_A, 5)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_SCKOUT_AF   GPIO_AF13

#define CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT      GPIO_PIN(PORT_A, 4)
#define CPU_STM32WL_SUBGHZSPI_DEBUG_NSSOUT_AF   GPIO_AF13
/** @} */

/**
 * @defgroup        cpu_stm32_wl_debug_subghz_radio STM32WL Sub-GHz Radio debug pins
 * @ingroup         cpu_stm32_wl_debug
 * @{
 */
#define CPU_STM32WL_SUBGHZ_RF_BUSY              GPIO_PIN(PORT_A, 12)
#define CPU_STM32WL_SUBGHZ_RF_BUSY_AF           GPIO_AF6

#define CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET      GPIO_PIN(PORT_A, 11)
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_NRESET_AF   GPIO_AF13

#define CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY     GPIO_PIN(PORT_B, 2)
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_SMPSRDY_AF  GPIO_AF13

#define CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY      GPIO_PIN(PORT_B, 4)
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_LDORDY_AF   GPIO_AF13

#define CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY    GPIO_PIN(PORT_A, 10)
#define CPU_STM32WL_SUBGHZ_DEBUG_RF_HSE32RDY_AF GPIO_AF13
/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_WL_PERIPH_CPU_H */
/** @} */
