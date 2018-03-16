﻿/*
 * Copyright (C) Lorenz Hüther, Mathias Detmers
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
  * @brief      Unit tests for pkg cn-cbor.
  *
  * @author     Lorenz Hüther <lorenz.huether@uni-bremen.de>
  * @author     Mathias Detmers <detmat@uni-bremen.de
  */

#define EBUF_SIZE 32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cn-cbor/cn-cbor.h"
#include "embUnit.h"

typedef struct buffer {
    size_t size;
    unsigned char *pntr;
} buffer_t;

typedef struct {
    char *hex;
    cn_cbor_error err;
} cbor_failure;

static cn_cbor *cbor;
static buffer_t pbuf;
static size_t test, offs;
static unsigned char ebuf[EBUF_SIZE];
static cn_cbor_errback errb;

static void setup_cn_cbor(void)
{
    cbor = NULL;
    test = 0;
    offs = 0;
    memset(ebuf, '\0', EBUF_SIZE);
}

static void teardown_cn_cbor(void)
{
        free(pbuf.pntr);
        cn_cbor_free(cbor);
}

static bool parse_hex(char *inpt)
{
    int strl = strlen(inpt);
    size_t offs;

    if (strl % 2 != 0) {
        pbuf.size = -1;
        pbuf.pntr = NULL;
        return false;
    }

    pbuf.size = strl / 2;
    pbuf.pntr = malloc(pbuf.size);

    for (offs = 0; offs < pbuf.size; offs++) {
        sscanf(inpt + (2 * offs), "%02hhx", &pbuf.pntr[offs]);
    }

    return true;
}

static void test_parse(void)
{
    char *tests[] = {
        "00",                       // 0
        "01",                       // 1
        "17",                       // 23
        "1818",                     // 24
        "190100",                   // 256
        "1a00010000",               // 65536
#ifndef CBOR_NO_LL
        "1b0000000100000000",       // 4294967296
#endif /* CBOR_NO_LL */

        "20",                       // -1
        "37",                       // -24
        "3818",                     // -25
        "390100",                   // -257
        "3a00010000",               // -65537
#ifndef CBOR_NO_LL
        "3b0000000100000000",       // -4294967297
#endif /* CBOR_LL */
        "4161",                     // h"a"
        "6161",                     // "a"
        "80",                       // []
        "8100",                     // [0]
        "820102",                   // [1,2]
        "818100",                   // [[0]]
        "a1616100",                 // {"a":0}
        "d8184100",                 // tag
        "f4",                       // false
        "f5",                       // true
        "f6",                       // null
        "f7",                       // undefined
        "f8ff",                     // simple(255)

#ifndef CBOR_NO_FLOAT
        "f93c00",                   // 1.0
        "f9bc00",                   // -1.0
        "f903ff",                   // 6.097555160522461e-05
        "f90400",                   // 6.103515625e-05
        "f907ff",                   // 0.00012201070785522461
        "f90800",                   // 0.0001220703125
        "fa47800000",               // 65536.0
        "fb3ff199999999999a",       // 1.1
        "f97e00",                   // NaN
#endif /* CBOR_NO_FLOAT */

        "5f42010243030405ff",       // (_ h'0102', h'030405')
        "7f61616161ff",             // (_ "a", "a")
        "9fff",                     // [_ ]
        "9f9f9fffffff",             // [_ [_ [_ ]]]
        "9f009f00ff00ff",           // [_ 0, [_ 0], 0]
        "bf61610161629f0203ffff",   // {_ "a": 1, "b": [_ 2, 3]}
    };

    for (test = 0; test < sizeof(tests) / sizeof(char*); test++) {
        TEST_ASSERT(parse_hex(tests[test]));
        errb.err = CN_CBOR_NO_ERROR;

        cbor = cn_cbor_decode(pbuf.pntr, pbuf.size, &errb);
        TEST_ASSERT_EQUAL_INT(errb.err, CN_CBOR_NO_ERROR);
        TEST_ASSERT_NOT_NULL(cbor);

        cn_cbor_encoder_write(ebuf, 0, sizeof(ebuf), cbor);
        for (offs = 0; offs < pbuf.size; offs++) {
            TEST_ASSERT_EQUAL_INT(pbuf.pntr[offs], ebuf[offs]);
        }
    }
}

static void test_errors(void)
{
    cbor_failure tests[] = {
        {"81", CN_CBOR_ERR_OUT_OF_DATA},
        {"0000", CN_CBOR_ERR_NOT_ALL_DATA_CONSUMED},
        {"bf00ff", CN_CBOR_ERR_ODD_SIZE_INDEF_MAP},
        {"ff", CN_CBOR_ERR_BREAK_OUTSIDE_INDEF},
        {"1f", CN_CBOR_ERR_MT_UNDEF_FOR_INDEF},
        {"1c", CN_CBOR_ERR_RESERVED_AI},
        {"7f4100", CN_CBOR_ERR_WRONG_NESTING_IN_INDEF_STRING},
    };

    cn_cbor inv = {CN_CBOR_INVALID, 0, {0}, 0, NULL, NULL, NULL, NULL};

    TEST_ASSERT_EQUAL_INT(-1, cn_cbor_encoder_write(ebuf, 0, sizeof(ebuf),
            &inv));

    for (offs = 0; offs < sizeof(tests) / sizeof(cbor_failure); offs++) {
        TEST_ASSERT(parse_hex(tests[offs].hex));

        cbor = cn_cbor_decode(pbuf.pntr, pbuf.size, &errb);
        TEST_ASSERT_NULL(cbor);
        TEST_ASSERT_EQUAL_INT(errb.err, tests[offs].err);
    }
}

TestRef test_cn_cbor(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_parse),
        new_TestFixture(test_errors)
    };

    EMB_UNIT_TESTCALLER(tests_cn_cbor, setup_cn_cbor, teardown_cn_cbor, fixtures);
    return (TestRef) & tests_cn_cbor;
}

void tests_cn_cbor(void)
{
    TESTS_RUN(test_cn_cbor());
}
