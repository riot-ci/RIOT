/*
 * Copyright (C) 2018 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_common_silabs_drivers_aem Advanced energy monitor driver
 * @ingroup     boards_common_silabs_drivers
 * @{
 *
 * @file
 * @brief       Implementations of the advanced energy monitor driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#ifndef AEM_H
#define AEM_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the advanced energy monitor.
 *
 * This must be performed as one of the first initializations, to provide
 * output as soon as possible.
 */
void aem_init(void);

#ifdef __cplusplus
}
#endif

#endif /* AEM_H */
/** @} */
