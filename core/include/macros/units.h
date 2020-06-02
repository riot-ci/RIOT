/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_macros
 * @{
 *
 * @file
 * @brief       Unit helper macros
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef MACROS_UNITS_H
#define MACROS_UNITS_H

#ifdef __cplusplus
 extern "C" {
#endif

/* KiB, MiB, GiB */
#define KiB(x) ((unsigned long long)(x) << 10)
#define MiB(x) (KiB(x) << 10)
#define GiB(x) (MiB(x) << 10)

/* kHz, MHz */
#define KHZ(x)    ((x) * 1000ULL)
#define MHZ(x) (KHZ(x) * 1000ULL)

#ifdef __cplusplus
}
#endif

#endif /* MACROS_UNITS_H */
/** @} */
