/*
 * Copyright (C) 2017 Freie Universität Berlin
 *               2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f7 STM32F7
 * @brief           STM32F7 specific code
 * @ingroup         cpu
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.pertersen@fu-berlin.de>
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef CPU_CONF_H
#define CPU_CONF_H

/* vendor header includes */
#if defined(CPU_MODEL_STM32F746ZG)
#include <stm32f746xx.h>
#elif defined(CPU_MODEL_STM32F767ZI)
#include <stm32f767xx.h>
#elif defined(CPU_MODEL_STM32F769NI)
#include <stm32f769xx.h>
#elif defined(CPU_MODEL_STM32F722ZE)
#include <stm32f722xx.h>
#endif

#include "cpu_conf_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#if defined(CPU_MODEL_STM32F746ZG)
#define CPU_IRQ_NUMOF                   (98U)
#elif defined(CPU_MODEL_STM32F769NI) || defined(CPU_MODEL_STM32F767ZI)
#define CPU_IRQ_NUMOF                   (110U)
#elif defined(CPU_MODEL_STM32F722ZE)
#define CPU_IRQ_NUMOF                   (104U)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
