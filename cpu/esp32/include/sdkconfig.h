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
 * @brief       Default compiled in configuration
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

#ifndef CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ 80
#endif

#ifndef CONFIG_CONSOLE_UART_NUM
#define CONFIG_CONSOLE_UART_NUM 0
#endif
#ifndef CONFIG_CONSOLE_UART_BAUDRATE
#define CONFIG_CONSOLE_UART_BAUDRATE 115200
#endif

#define CONFIG_BT_ENABLED               0
#define CONFIG_BT_RESERVE_DRAM          0
#define CONFIG_TRACEMEM_RESERVE_DRAM    0
#define CONFIG_ULP_COPROC_RESERVE_MEM   0

#define CONFIG_ESP32_PHY_MAX_TX_POWER   20

#define CONFIG_ESP32_RTC_XTAL_BOOTSTRAP_CYCLES  100
#define CONFIG_ESP32_RTC_CLK_CAL_CYCLES         1024

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

#define CONFIG_SPI_FLASH_ROM_DRIVER_PATCH       1
#define CONFIG_NEWLIB_NANO_FORMAT               1
#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* SDKCONFIG_H */
