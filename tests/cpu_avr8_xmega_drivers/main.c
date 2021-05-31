/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
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
 * @brief       Main test application for ATxmega
 *
 * @author      Gerson Fernando Budke <nandojve@gmail.com>
 *
 * @}
 */

#include "stdio.h"
#include "cpu_tests.h"

int main(void)
{
#if defined(MODULE_PERIPH_EBI)
    puts("Start EBI tests");
    ebi_tests();
#endif

    return 0;
}
