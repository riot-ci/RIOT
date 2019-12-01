/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 * @file
 * @brief       Default configuration for a AT86RF233 device
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF233_PARAMS_H
#define AT86RF233_PARAMS_H

#include "kernel_defines.h"

#if IS_USED(MODULE_AT86RF233)

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Additional params for AT86RF233 transceivers could be defined here */
/* A user could #define AT86RF233_PARAMS right here */

#ifdef __cplusplus
}
#endif

#endif /* IS_USED(MODULE_AT86RF233) */

#endif /* AT86RF233_PARAMS_H */
/** @} */
