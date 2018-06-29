/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Default compiled in configuration compatible to the ESP-IDF
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef SDKCONFIG_H
#define SDKCONFIG_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Default console configuration (change only CONFIG_CONSOLE_UART_BAUDRATE)
 */
/** CONFIG_CONSOLE_UART_NUM is also used as UART_STDIO_BAUDRATE */
#ifndef CONFIG_CONSOLE_UART_NUM
#define CONFIG_CONSOLE_UART_NUM 0
#endif
#ifndef CONFIG_CONSOLE_UART_BAUDRATE
#define CONFIG_CONSOLE_UART_BAUDRATE 115200
#endif

/**
 * Log output configuration
 */
#define CONFIG_LOG_DEFAULT_LEVEL 3

/**
 * ESP32 specific configuration (change only CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
 */
/** possible values are 2, 40, 80, 160 and 240 */
#ifndef CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ 80
#endif
#define CONFIG_ESP32_RTC_XTAL_BOOTSTRAP_CYCLES  100
#define CONFIG_ESP32_RTC_CLK_CAL_CYCLES         1024

/**
 * System specific configuration (DO NOT CHANGE)
 */
#define CONFIG_TRACEMEM_RESERVE_DRAM            0
#define CONFIG_ULP_COPROC_RESERVE_MEM           0

#define CONFIG_SYSTEM_EVENT_QUEUE_SIZE          32
#define CONFIG_SYSTEM_EVENT_TASK_STACK_SIZE     2048
#define CONFIG_NUMBER_OF_UNIVERSAL_MAC_ADDRESS  4

#define CONFIG_NEWLIB_NANO_FORMAT               0

/**
 * Bluetooth configuration (DO NOT CHANGE IT)
 */
#define CONFIG_BT_ENABLED                       0
#define CONFIG_BT_RESERVE_DRAM                  0

/**
 * Physical layer configuration
 */
#define CONFIG_ESP32_PHY_MAX_TX_POWER           20

/**
 * SPI RAM configuration (DO NOT CHANGE)
 */
#ifdef  SPI_RAM_USED
#define CONFIG_SPIRAM_SUPPORT                   1
#else
#define CONFIG_SPIRAM_SUPPORT                   0
#endif
#define CONFIG_SPIRAM_SPEED_40M                 1
#define CONFIG_SPIRAM_SIZE                      4194304
#define CONFIG_SPIRAM_BOOT_INIT                 1
#define CONFIG_SPIRAM_USE_MALLOC                1
#define CONFIG_SPIRAM_TYPE_ESPPSRAM32           1
#define CONFIG_SPIRAM_MEMTEST                   1
#define CONFIG_SPIRAM_CACHE_WORKAROUND          1
#define CONFIG_SPIRAM_MALLOC_ALWAYSINTERNAL     16384
#define CONFIG_SPIRAM_MALLOC_RESERVE_INTERNAL   32768

/**
 * SPI Flash driver configuration (DO NOT CHANGE)
 */
#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH       1

/**
 * Ethernet driver configuration
 */
#define CONFIG_DMA_RX_BUF_NUM                   10
#define CONFIG_DMA_TX_BUF_NUM                   10
#define CONFIG_EMAC_TASK_PRIORITY               20

/**
 * Serial flasher config (DO NOT CHANGE)
 */
#define CONFIG_ESPTOOLPY_FLASHFREQ_40M          1
#if defined(FLASH_MODE_QIO)
#define CONFIG_FLASHMODE_QIO                    1
#elif defined(FLASH_MODE_QOUT)
#define CONFIG_FLASHMODE_QOUT                   1
#elif defined(FLASH_MODE_DIO)
#define CONFIG_FLASHMODE_DIO                    1
#else
#define CONFIG_FLASHMODE_DOUT                   1
#endif

/**
 * Wi-Fi driver configuration (DO NOT CHANGE)
 */
#define CONFIG_ESP32_WIFI_TX_BUFFER_TYPE        1
#define CONFIG_ESP32_WIFI_STATIC_TX_BUFFER      0
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER     1
#define CONFIG_ESP32_WIFI_DYNAMIC_TX_BUFFER_NUM 48 // 32
#define CONFIG_ESP32_WIFI_STATIC_RX_BUFFER_NUM  10
#define CONFIG_ESP32_WIFI_DYNAMIC_RX_BUFFER_NUM 64 // 32
#define CONFIG_ESP32_WIFI_AMPDU_TX_ENABLED      1
#define CONFIG_ESP32_WIFI_AMPDU_RX_ENABLED      1
#define CONFIG_ESP32_WIFI_TX_BA_WIN             6
#define CONFIG_ESP32_WIFI_RX_BA_WIN             6
#define CONFIG_ESP32_WIFI_NVS_ENABLED           0
#define CONFIG_ESP32_WIFI_CSI_ENABLED           0
#define CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_0 1
#define CONFIG_ESP32_WIFI_TASK_PINNED_TO_CORE_1 0

/**
 * EMAC driver configuration (DO NOT CHANGE)
 */
#define CONFIG_EMAC_L2_TO_L3_RX_BUF_MODE        1


#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SDKCONFIG_H */
