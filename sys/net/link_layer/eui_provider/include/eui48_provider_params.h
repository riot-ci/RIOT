/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_eui_provider
 *
 * @file
 * @brief       EUI-48 address provider default values
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */
#ifndef EUI48_PROVIDER_PARAMS_H
#define EUI48_PROVIDER_PARAMS_H

#include "board.h"
#include "net/eui_provider.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EUI48_PROVIDER_FUNC
#define EUI48_PROVIDER_FUNC   NULL

/* on EUI providers defined */
#ifndef EUI48_PROVIDER_PARAMS
#define EUI48_PROVIDER_NUMOF 0U
#endif
#endif

#ifndef EUI48_PROVIDER_ARG
#define EUI48_PROVIDER_ARG    NULL
#endif

#ifndef EUI48_PROVIDER_TYPE
#define EUI48_PROVIDER_TYPE   NETDEV_ANY
#endif

#ifndef EUI48_PROVIDER_INDEX
#define EUI48_PROVIDER_INDEX  NETDEV_INDEX_ANY
#endif

#ifndef EUI48_PROVIDER_PARAMS
#define EUI48_PROVIDER_PARAMS     {                                \
                                 .provider = EUI48_PROVIDER_FUNC,  \
                                 .arg      = EUI48_PROVIDER_ARG,   \
                                 .type     = EUI48_PROVIDER_TYPE,  \
                                 .index    = EUI48_PROVIDER_INDEX, \
                                },
#endif


/**
 * @name    EUI-48 sources on the board
 *
 * @{
 */
static const eui48_conf_t eui48_conf[] = {
    EUI48_PROVIDER_PARAMS
};

#ifndef EUI48_PROVIDER_NUMOF
#define EUI48_PROVIDER_NUMOF    ARRAY_SIZE(eui48_conf)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* EUI48_PROVIDER_PARAMS_H */
