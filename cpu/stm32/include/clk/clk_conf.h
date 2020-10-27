/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Main header for STM32 clock configuration
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
*/

#ifndef CLK_CLK_CONF_H
#define CLK_CLK_CONF_H

#include "kernel_defines.h"
#include "macros/units.h"

#if defined(CPU_FAM_STM32F0)
#include "f0/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32F1) || defined(CPU_FAM_STM32F3)
#include "f1f3/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7)
#include "f2f4f7/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32G0)
#include "g0/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32G4)
#include "g4/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#include "l0l1/cfg_clock_default.h"
#elif defined(CPU_FAM_STM32L4) || defined(CPU_FAM_STM32L5) || \
      defined(CPU_FAM_STM32WB)
#include "l4l5wb/cfg_clock_default.h"
#else
#error "No clock configuration available"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* CLK_CLK_CONF_H */
/** @} */
