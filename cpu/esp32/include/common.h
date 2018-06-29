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
 * @brief       Common helper macros
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef COMMON_H
#define COMMON_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include "rom/ets_sys.h"

#define asm __asm__

/** string representation of x */
#ifndef XTSTR
#define _XTSTR(x)    # x
#define XTSTR(x)    _XTSTR(x)
#endif /* XSTR */

#if !defined(ICACHE_FLASH)
#ifndef ICACHE_RAM_ATTR
/** Places the code with this attribute in the IRAM. */
#define ICACHE_RAM_ATTR  __attribute__((section(".iram0.text")))
#endif
#else /* ICACHE_FLASH */
#ifndef ICACHE_RAM_ATTR
#define ICACHE_RAM_ATTR
#endif
#endif /* ICACHE_FLASH */

/** Print out a message that function is not yet implementd */
#define NOT_YET_IMPLEMENTED()     LOG_INFO("%s not yet implemented\n", __func__)
/** Print out a message that function is not supported */
#define NOT_SUPPORTED()           LOG_INFO("%s not supported\n", __func__)

/** definitions for source code compatibility with ESP-IDF */
#define ESP_EARLY_LOGE(tag, fmt, ...) ets_printf("%s(err): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_EARLY_LOGW(tag, fmt, ...) ets_printf("%s(warn): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_EARLY_LOGI(tag, fmt, ...) ets_printf("%s(info): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGE(tag, fmt, ...) ets_printf("%s(err): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGW(tag, fmt, ...) ets_printf("%s(warn): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGI(tag, fmt, ...) ets_printf("%s(info): " fmt "\n", tag, ##__VA_ARGS__)

#if ENABLE_DEBUG
/**
  * @brief  Parameter check with return a value.
  *
  * If ENABLE_DEBUG is true, the macro checks a condition and returns with a value
  * if the condition is not fulfilled.
  * @param  cond    the condition
  * @param  err     the return value in the case the condition is not fulfilled.
  */
#define CHECK_PARAM_RET(cond,err)   if (!(cond)) \
                                    { \
                                        DEBUG("%s parameter condition (" #cond ") " \
                                              "not fulfilled\n", __func__); \
                                        return err; \
                                    }

/**
 * @brief  Parameter check without return value.
 *
 * If ENABLE_DEBUG is true, the macro checks a condition and returns without a
 * value if the condition is not fulfilled.
 * @param  cond    the condition
 */
#define CHECK_PARAM(cond)   if (!(cond)) \
                            { \
                                DEBUG("%s parameter condition (" #cond ") " \
                                      "not fulfilled\n", __func__); \
                                return; \
                            }

#define ESP_EARLY_LOGD(tag, fmt, ...) ets_printf("%s(dbg): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_EARLY_LOGV(tag, fmt, ...) ets_printf("%s: " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) ets_printf("%s(dbg): " fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGV(tag, fmt, ...) ets_printf("%s: " fmt "\n", tag, ##__VA_ARGS__)

#else /* ENABLE_DEBUG */

#define CHECK_PARAM_RET(cond,err) if (!(cond)) return err;
#define CHECK_PARAM(cond)         if (!(cond)) return;

#define ESP_EARLY_LOGD( tag, format, ... ) (void)tag
#define ESP_EARLY_LOGV( tag, format, ... ) (void)tag
#define ESP_LOGD( tag, format, ... ) (void)tag
#define ESP_LOGV( tag, format, ... ) (void)tag

#endif /* ENABLE_DEBUG */

/** gives the minimum of a and b */
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

/** gives the maximum of a and b */
#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/**
  * @brief  funcion name mappings for source code compatibility with ESP8266 port
  * @{
  */
#define system_get_cpu_freq     ets_get_cpu_frequency
#define system_update_cpu_freq  ets_update_cpu_frequency
/** @} */

/** @} */

/** microseconds per millisecond */
#ifndef USEC_PER_MSEC
#define USEC_PER_MSEC 1000UL
#endif

#ifndef MSEC_PER_SEC
#define MSEC_PER_SEC  1000UL
#endif

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

#endif /* COMMON_H */
