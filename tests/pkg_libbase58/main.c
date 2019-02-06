/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 *
 * @file
 * @brief      Tests for pkg libbase58
 *
 * @author     Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "libbase58.h"
#include "embUnit.h"

static void setUp(void)
{
}

static void test_libbase58_01(void)
{
    const char source[] = "base 58 test string";
    const char encoded[] = "K1cpEhnSEL8mbBfpp7f6z3dSZ4";
    char target[64] = {0};
    size_t target_len = sizeof(target);

    b58enc(target, &target_len, source, sizeof(source)-1);
    TEST_ASSERT_EQUAL_INT(target_len, sizeof(encoded));
    TEST_ASSERT(strcmp(target, encoded) == 0);
}

Test *tests_libbase58(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_libbase58_01),
    };

    EMB_UNIT_TESTCALLER(libbase58_tests, setUp, NULL, fixtures);
    return (Test *)&libbase58_tests;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_libbase58());
    TESTS_END();
    return 0;
}
