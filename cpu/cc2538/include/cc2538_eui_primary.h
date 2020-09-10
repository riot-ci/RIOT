/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_cc2538
 * @{
 *
 * @file
 * @brief           CC2538 EUI-64 provider
 *
 * @author          Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef CC2538_EUI_PRIMARY
#define CC2538_EUI_PRIMARY

#include "cc2538_rf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    CC2538 comes with an EUI-64
 * @{
 */
#define EUI64_PROVIDER_FUNC   cc2538_get_eui64_primary
#define EUI64_PROVIDER_TYPE   NETDEV_CC2538
#define EUI64_PROVIDER_INDEX  0
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC2538_EUI_PRIMARY */
/** @} */
