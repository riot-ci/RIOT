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
 * @brief       Default application specific SDK configuration
 *
 * SDK configuration file ```$(RIOTCPU)/$(CPU)/include/sdk_conf.h```
 * includes an application specific SDK configuration file ```sdk_conf_app.h```
 * from application source directory ```$(APPDIR)``` which allows to override
 * the default configuration. If there is no such application specific SDK
 * configuration file ```sdk_conf_app.h``` in the application source directory,
 * this file is used as default configuration.
 *
 * This file also documents all definitions that can be overriden by the
 * application specific SDK configuration file.
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef DEFAULT_SDK_CONF_APP_H
#define DEFAULT_SDK_CONF_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Defines the CPU frequency [vallues = 2, 40, 80, 160 and 240]
 */
#define CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ   80

/**
 * @brief   Defines the UART baudrate also used as UART_STDIO_BAUDRATE
 */
#define CONFIG_CONSOLE_UART_BAUDRATE    115200

#ifdef __cplusplus
}
#endif

#endif /* DEFAULT_SDK_CONF_APP_H */
