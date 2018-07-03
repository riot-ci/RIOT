/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef FREERTOS_FREERTOS_H
#define FREERTOS_FREERTOS_H

#ifndef DOXYGEN

#include "portmacro.h"
#include "semphr.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int32_t  BaseType_t;
typedef uint32_t UBaseType_t;

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_FREERTOS_H */
