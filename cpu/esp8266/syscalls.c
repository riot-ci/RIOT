/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Implementation of required system calls
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include "sdk/sdk.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/**
 * @name Other system functions
 */

void syscalls_init_arch(void)
{
}

uint32_t system_get_time(void)
{
    return phy_get_mactime();
}
