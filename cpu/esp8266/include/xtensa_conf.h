/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Xtensa ASM code specific configuration options
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef XTENSA_CONF_H
#define XTENSA_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Xtensa ASM code specific default stack sizes
 * @{
 */
#if defined(SDK_INT_HANDLING)
#define ISR_STACKSIZE                 (8)
#else
#define ISR_STACKSIZE                 (2048)
#endif
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* XTENSA_CONF_H */
/** @} */
