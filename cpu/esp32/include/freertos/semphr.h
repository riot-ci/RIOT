/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * FreeRTOS to RIOT-OS adaption module for source code compatibility
 */

#ifndef FREERTOS_SEMPHR_H
#define FREERTOS_SEMPHR_H

#ifndef DOXYGEN

#include "stdlib.h"

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SemaphoreHandle_t mutex_t*

static inline SemaphoreHandle_t xSemaphoreCreateMutex(void)
{
    mutex_t* _tmp = (mutex_t*)malloc (sizeof(mutex_t));
    mutex_init(_tmp);
    return _tmp;
}

static inline void vSemaphoreDelete( SemaphoreHandle_t xSemaphore )
{
    free(xSemaphore);
}

#define vPortCPUInitializeMutex(m)  mutex_init(m)
#define xSemaphoreTake(s,to)        mutex_lock(s)
#define xSemaphoreGive(s)           mutex_unlock(s)

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* FREERTOS_SEMPHR_H */
