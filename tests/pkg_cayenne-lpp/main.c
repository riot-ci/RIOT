/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     test
 * @{
 *
 * @file
 * @brief       Cayenne Low Power Payload example application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>

#include "cayenne-lpp.h"

static cayenne_lpp_t lpp;

int main(void)
{
    puts("Cayenne LPP test application");

    cayenne_lpp_add_temperature(&lpp, 3, 27.2);
    cayenne_lpp_add_temperature(&lpp, 5, 25.5);
    for (uint8_t i = 0; i < lpp.cursor; ++i) {
        printf("%02X", lpp.buffer[i]);
    }
    puts("");

    return 0;
}
