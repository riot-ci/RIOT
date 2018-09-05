/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */
#include <string.h>
#include <errno.h>

#include "embUnit.h"

#include "mtd.h"
#include "mtd_flashpage.h"

static mtd_dev_t _dev = MTD_FLASHPAGE_INIT_VAL(8);
static mtd_dev_t *dev = &_dev;

static void setup_teardown(void)
{
    mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, dev->pages_per_sector * dev->page_size);
}

static void test_mtd_init(void)
{
    int ret = mtd_init(dev);
    TEST_ASSERT_EQUAL_INT(0, ret);
}

static void test_mtd_erase(void)
{
    /* Erase last sector */
    int ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, FLASHPAGE_SIZE);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase with wrong size (less than sector size) */
    ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Unaligned erase */
    ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE + dev->page_size, dev->page_size);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);

    /* Erase 2 last sectors */
    ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 2) * FLASHPAGE_SIZE,
                    FLASHPAGE_SIZE * 2);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Erase out of memory area */
    ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE,
                    FLASHPAGE_SIZE * 2);
    TEST_ASSERT_EQUAL_INT(-EOVERFLOW, ret);
}

static void test_mtd_write_erase(void)
{
    const char buf[] = "ABCDEFGHIJK";
    uint8_t buf_empty[] = {0xff, 0xff, 0xff};
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    int ret = mtd_write(dev, buf, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), ret);

    ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    uint8_t expected[sizeof(buf_read)];
    memset(expected, 0, sizeof(expected));
    ret = mtd_read(dev, buf_read, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(sizeof(buf_read), ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(expected, buf_read, sizeof(buf_read)));
}

static void test_mtd_write_read(void)
{
    const char buf[] = "ABCDEFG";
    uint8_t buf_empty[] = {0, 0, 0, 0};
    char buf_read[sizeof(buf) + sizeof(buf_empty)];
    memset(buf_read, 0, sizeof(buf_read));

    /* Basic write / read */
    int ret = mtd_write(dev, buf, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), ret);

    ret = mtd_read(dev, buf_read, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(sizeof(buf_read), ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read, sizeof(buf)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read + sizeof(buf), sizeof(buf_empty)));

    ret = mtd_erase(dev, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, dev->pages_per_sector * dev->page_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* Unaligned write / read */
    ret = mtd_write(dev, buf, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE + sizeof(buf_empty), sizeof(buf));
    TEST_ASSERT_EQUAL_INT(sizeof(buf), ret);

    ret = mtd_read(dev, buf_read, (FLASHPAGE_NUMOF - 1) * FLASHPAGE_SIZE, sizeof(buf_read));
    TEST_ASSERT_EQUAL_INT(sizeof(buf_read), ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf_empty, buf_read, sizeof(buf_empty)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(buf, buf_read + sizeof(buf_empty), sizeof(buf)));
}

Test *tests_mtd_flashpage_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),
        new_TestFixture(test_mtd_write_erase),
        new_TestFixture(test_mtd_write_read),
    };

    EMB_UNIT_TESTCALLER(mtd_flashpage_tests, setup_teardown, setup_teardown, fixtures);

    return (Test *)&mtd_flashpage_tests;
}

void tests_mtd_flashpage(void)
{
    TESTS_RUN(tests_mtd_flashpage_tests());
}
/** @} */
