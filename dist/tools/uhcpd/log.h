/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_util
 * @{
 *
 * @file
 * @brief       System logging dummy header
 *
 * This header offers a bunch of "LOG_*" functions that just use printf,
 * without honouring a verbosity level.
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef LOG_H
#define LOG_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Logging convenience defines
 * @{
 */
#define LOG_ERROR(...) printf(__VA_ARGS__)
#define LOG_WARNING(...) printf(__VA_ARGS__)
#define LOG_INFO(...) printf(__VA_ARGS__)
#define LOG_DEBUG(...) printf(__VA_ARGS__)
/** @} */

#endif /* LOG_H */
/** @} */
