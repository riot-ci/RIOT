/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Emlearn test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>
#include "sonar.h"
#include "blob/digit.h"

int main(void)
{
    printf("Predicted digit: %" PRIi32 "\n", sonar_predict((const float *)digit, digit_len >> 2));

    return 0;
}
