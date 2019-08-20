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
 * @brief       Common log macros
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 */

#ifndef ESP_COMMON_LOG_H
#define ESP_COMMON_LOG_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>

#include "log.h"

extern uint32_t system_get_time_ms (void);

#if MODULE_ESP_LOG_COLOR

#define LOG_COLOR_RED     "31"
#define LOG_COLOR_GREEN   "32"
#define LOG_COLOR_BROWN   "33"
#define LOG_COLOR(COLOR)  "\033[0;" COLOR "m"
#define LOG_RESET_COLOR   "\033[0m"
#define LOG_COLOR_E       LOG_COLOR(LOG_COLOR_RED)
#define LOG_COLOR_W       LOG_COLOR(LOG_COLOR_BROWN)
#define LOG_COLOR_I       LOG_COLOR(LOG_COLOR_GREEN)
#define LOG_COLOR_D
#define LOG_COLOR_V

#else /* MODULE_ESP_LOG_COLOR */

#define LOG_COLOR_E
#define LOG_COLOR_W
#define LOG_COLOR_I
#define LOG_COLOR_D
#define LOG_COLOR_V
#define LOG_RESET_COLOR

#endif /* MODULE_ESP_LOG_COLOR */

#define LOG_FORMAT(letter, format)  LOG_COLOR_ ## letter #letter " (%d) [%s] " format LOG_RESET_COLOR

#define LOG_TAG(level, letter, tag, format, ...) \
                 do { \
                    if ((unsigned)level <= (unsigned)LOG_LEVEL) { \
                        printf(LOG_FORMAT(letter, format), system_get_time_ms(), tag, ##__VA_ARGS__); \
                    } \
                } while(0)

#define LOG_TAG_EARLY(level, letter, tag, format, ...) \
                do { \
                    if (LOG_LEVEL >= level) { \
                        ets_printf(LOG_FORMAT(letter, format), system_get_time_ms(), tag, ##__VA_ARGS__); \
                    } \
                } while(0)

/**
 * Override LOG_* definitions with a tagged version. By default the function
 * name is used as tag.
 */
#ifndef MODULE_LOG_PRINTFNOFORMAT
#undef LOG_ERROR
#undef LOG_INFO
#undef LOG_WARNING
#undef LOG_DEBUG
#define LOG_ERROR(format, ...)   LOG_TAG(LOG_ERROR  , E, __func__, format, ##__VA_ARGS__)
#define LOG_WARNING(format, ...) LOG_TAG(LOG_WARNING, W, __func__, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...)    LOG_TAG(LOG_INFO   , I, __func__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...)   LOG_TAG(LOG_DEBUG  , D, __func__, format, ##__VA_ARGS__)
#endif

/** Tagged LOG_* definitions */
#define LOG_TAG_ERROR(tag, format, ...)   LOG_TAG(LOG_ERROR  , E, tag, format, ##__VA_ARGS__)
#define LOG_TAG_WARNING(tag, format, ...) LOG_TAG(LOG_WARNING, W, tag, format, ##__VA_ARGS__)
#define LOG_TAG_INFO(tag, format, ...)    LOG_TAG(LOG_INFO   , I, tag, format, ##__VA_ARGS__)
#define LOG_TAG_DEBUG(tag, format, ...)   LOG_TAG(LOG_DEBUG  , D, tag, format, ##__VA_ARGS__)

/** definitions for source code compatibility with ESP-IDF */
#define ESP_EARLY_LOGE(tag, format, ...) LOG_TAG_EARLY(LOG_ERROR  , E, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGW(tag, format, ...) LOG_TAG_EARLY(LOG_WARNING, W, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGI(tag, format, ...) LOG_TAG_EARLY(LOG_INFO   , I, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGE(tag, format, ...) LOG_TAG(LOG_ERROR  , E, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGW(tag, format, ...) LOG_TAG(LOG_WARNING, W, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGI(tag, format, ...) LOG_TAG(LOG_INFO   , I, tag, format "\n", ##__VA_ARGS__)

#if ENABLE_DEBUG

#define ESP_EARLY_LOGD(tag, format, ...) LOG_TAG_EARLY(LOG_DEBUG, D, tag, format "\n", ##__VA_ARGS__)
#define ESP_EARLY_LOGV(tag, format, ...) LOG_TAG_EARLY(LOG_ALL  , V, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGD(tag, format, ...) LOG_TAG(LOG_DEBUG, D, tag, format "\n", ##__VA_ARGS__)
#define ESP_LOGV(tag, format, ...) LOG_TAG(LOG_ALL  , V, tag, format "\n", ##__VA_ARGS__)

#else /* ENABLE_DEBUG */

#define ESP_EARLY_LOGD( tag, format, ... ) (void)tag
#define ESP_EARLY_LOGV( tag, format, ... ) (void)tag
#define ESP_LOGD( tag, format, ... ) (void)tag
#define ESP_LOGV( tag, format, ... ) (void)tag

#endif /* ENABLE_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */

#endif /* ESP_COMMON_LOG_H */
