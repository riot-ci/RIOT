/**
Copyright (C) 2019, HAW Hamburg.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sys_metadata
 * @{
 * @file
 * @brief   Gets metadata from application
 * @author  Kevin Weiss <kevin.weiss@haw-hamburg.de>
 * @}
 */

#include <stdio.h>

void metadata_get(void)
{
    puts("{\"cmd\": \"metadata_get()\"}");
    printf("{\"data\": {\"APP_NAME\": \"%s\"}}\n", RIOT_APPLICATION);
    printf("{\"data\": {\"BOARD\": \"%s\"}}\n", RIOT_BOARD);
    printf("{\"data\": {\"CPU\": \"%s\"}}\n", RIOT_CPU);
#ifdef INTERFACE_VERSION
    printf("{\"data\": {\"IF_VERSION\": \"%s\"}}\n", INTERFACE_VERSION);
#endif
    printf("{\"data\": {\"MCU\": \"%s\"}}\n", RIOT_MCU);
    printf("{\"data\": {\"OS_VERSION\": \"%s\"}}\n", RIOT_VERSION);

    printf("{\"result\": \"SUCCESS\"}\n");
}
