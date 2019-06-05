/*
* Copyright (C) 2019 HAW Hamburg
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#include <string.h>
#include "embUnit.h"
#include "tests-credman.h"
#include "credentials.h"

#include "net/credman.h"

#define CREDMAN_TEST_TAG (1)

ecdsa_params_t exp_ecdsa_params = {
    .private_key = ecdsa_priv_key,
    .public_key = { .x = ecdsa_pub_key_x, .y = ecdsa_pub_key_y },
    .client_keys = NULL,
    .client_keys_size = 0,
};

psk_params_t exp_psk_params = {
    .id =  {
        .s = (void *)"RIOTer",
        .len = sizeof("RIOTer") - 1,
    },
    .key = {
        .s = (void *)"LGPLisyourfriend",
        .len = sizeof("LGPLisyourfriend") - 1,
    },
};

static void set_up(void)
{
    /* empty/reinit system buffer after every test */
    credman_init();
}

static void test_credman_add_credential(void)
{
    int ret;
    unsigned exp_count = 0;

    credman_credential_t credential = {
        .tag = CREDMAN_TEST_TAG,
        .type = CREDMAN_TYPE_PSK,
        .params = { .psk = &exp_psk_params }
    };

    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* add one credential */
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add_credential(&credential));
    TEST_ASSERT_EQUAL_INT(++exp_count, credman_get_used_count());

    /* add duplicate credential */
    ret = credman_add_credential(&credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_EXIST, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* add invalid credential params */
    credential.params.psk = NULL;
    ret = credman_add_credential(&credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_ERROR, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* fill the system credential buffer */
    credential.params.psk = &exp_psk_params;
    while (credman_get_used_count() < CREDMAN_MAX_CREDENTIALS) {
        /* increase tag number so that it is not recognized as duplicate */
        credential.tag++;
        TEST_ASSERT_EQUAL_INT(CREDMAN_OK, credman_add_credential(&credential));
        TEST_ASSERT_EQUAL_INT(++exp_count, credman_get_used_count());
    }

    /* add to full system credential buffer */
    credential.tag++;
    ret = credman_add_credential(&credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NO_SPACE, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());
}

static void test_credman_get_credential(void)
{
    int ret;
    credman_credential_t out_credential;
    credman_credential_t exp_credential = {
        .tag = CREDMAN_TEST_TAG,
        .type = CREDMAN_TYPE_ECDSA,
        .params = { .ecdsa = &exp_ecdsa_params }
    };

    /* get non-existing credential */
    ret = credman_get_credential(&out_credential, exp_credential.tag,
                                 exp_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, ret);

    ret = credman_add_credential(&exp_credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);

    ret = credman_get_credential(&out_credential, exp_credential.tag,
                                 exp_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(&out_credential, &exp_credential,
                                 sizeof(exp_credential)));
}

static void test_credman_delete_credential(void)
{
    int ret;
    unsigned exp_count = 0;
    credman_credential_t exp_credential = {
        .tag = CREDMAN_TEST_TAG,
        .type = CREDMAN_TYPE_ECDSA,
        .params = { .ecdsa = &exp_ecdsa_params }
    };

    /* delete non-existing credential */
    ret = credman_delete_credential(exp_credential.tag, exp_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());

    /* add a credential */
    ret = credman_add_credential(&exp_credential);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);
    TEST_ASSERT_EQUAL_INT(++exp_count, credman_get_used_count());

    /* delete a credential from system buffer */
    ret = credman_delete_credential(exp_credential.tag, exp_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_OK, ret);
    TEST_ASSERT_EQUAL_INT(--exp_count, credman_get_used_count());

    /* delete a deleted credential */
    ret = credman_delete_credential(exp_credential.tag, exp_credential.type);
    TEST_ASSERT_EQUAL_INT(CREDMAN_NOT_FOUND, ret);
    TEST_ASSERT_EQUAL_INT(exp_count, credman_get_used_count());
}

Test *tests_credman_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_credman_add_credential),
        new_TestFixture(test_credman_get_credential),
        new_TestFixture(test_credman_delete_credential),
    };

    EMB_UNIT_TESTCALLER(credman_tests,
                        set_up,
                        NULL, fixtures);

    return (Test *)&credman_tests;
}

void tests_credman(void)
{
    TESTS_RUN(tests_credman_tests());
}