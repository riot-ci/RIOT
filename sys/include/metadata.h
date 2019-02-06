/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */


/**
 * @defgroup    sys_metadata metadata
 * @ingroup     sys
 * @brief       Shows metadata of the application
 * @{
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifndef METADATA_H
#define METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prints information about metadata to stdio.
 */
void metadata_get(void);

#ifdef __cplusplus
}
#endif

#endif /* METADATA_H */
/** @} */
