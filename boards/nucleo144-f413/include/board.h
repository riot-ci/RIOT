/*
 * Copyright (C) 2016 Inria
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_nucleo144-f413 Nucleo144-F413
 * @ingroup     boards_nucleo144
 * @brief       Board specific files for the STM Nucleo144-f413 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo144-f413 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Vincent Dupont <vincent@otakeys.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "board_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_OVERHEAD     (6)
#define XTIMER_BACKOFF      (5)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
