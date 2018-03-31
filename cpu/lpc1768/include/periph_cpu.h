/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lpc1768
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include "cpu.h"
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   CPU provides own pm_off() function
 */
#define PROVIDES_PM_LAYERED_OFF

/**
 * @brief   Power management configuration
 */
#define PM_NUM_MODES    (3U)

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H */
/** @} */
