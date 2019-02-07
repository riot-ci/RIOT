/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_app_metadata app_metadata
 * @ingroup     sys
 * @brief       Module for the application metadata
 * @{
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifndef APP_METADATA_H
#define APP_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prints the application metadata in json
 */
void app_metadata_print_json(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_METADATA_H */
/** @} */
