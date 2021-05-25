#include <string.h>
#include "inttypes.h"
#include "edhoc/edhoc.h"
#include "embUnit.h"
#include "edhoc_keys.h"
#include "tinycrypt/sha256.h"

#define CRED_DB_SIZE        ARRAY_SIZE(cred_db)

static const uint8_t expected_m1[] = {
    0x0d, 0x00, 0x58, 0x20, 0x8d, 0x3e, 0xf5, 0x6d,
    0x1b, 0x75, 0x0a, 0x43, 0x51, 0xd6, 0x8a, 0xc2,
    0x50, 0xa0, 0xe8, 0x83, 0x79, 0x0e, 0xfc, 0x80,
    0xa5, 0x38, 0xa4, 0x44, 0xee, 0x9e, 0x2b, 0x57,
    0xe2, 0x44, 0x1a, 0x7c, 0x21
};

static const uint8_t msg2[] = {
    0x58, 0x20, 0x52, 0xfb, 0xa0, 0xbd, 0xc8, 0xd9,
    0x53, 0xdd, 0x86, 0xce, 0x1a, 0xb2, 0xfd, 0x7c,
    0x05, 0xa4, 0x65, 0x8c, 0x7c, 0x30, 0xaf, 0xdb,
    0xfc, 0x33, 0x01, 0x04, 0x70, 0x69, 0x45, 0x1b,
    0xaf, 0x35, 0x37, 0x4a, 0xa3, 0xf1, 0xbd, 0x5d,
    0x02, 0x8d, 0x19, 0xcf, 0x3c, 0x99
};

static const uint8_t expected_m3[] = {
    0x37, 0x52, 0xd5, 0x53, 0x5f, 0x31, 0x47, 0xe8,
    0x5f, 0x1c, 0xfa, 0xcd, 0x9e, 0x78, 0xab, 0xf9,
    0xe0, 0xa8, 0x1b, 0xbf
};

static uint8_t msg1[512];
static uint8_t msg3[512];

int _cred_cb(const uint8_t *k, size_t k_len, const uint8_t **o, size_t *o_len)
{
    for (uint8_t i = 0; i < (uint8_t)CRED_DB_SIZE; i++) {
        if (cred_db[i].id_len == k_len) {
            if (memcmp(cred_db[i].id, k, k_len) == 0) {
                *o = cred_db[i].cred;
                *o_len = cred_db[i].cred_len;
                return 0;
            }
        }
    }
    *o = NULL;
    *o_len = 0;
    return EDHOC_ERR_INVALID_CRED_ID;
}

static void setUp(void)
{
    /* Initialize */
    memset(msg1, '\0', sizeof(msg1));
    memset(msg3, '\0', sizeof(msg3));
}

static void test_msg3(void)
{
    cose_key_t authkey;
    cose_key_init(&authkey);
    edhoc_ctx_t ctx;
    edhoc_ctx_init(&ctx);
    edhoc_conf_t conf;
    edhoc_conf_init(&conf);

    TEST_ASSERT_EQUAL_INT(cose_key_from_cbor(&authkey, init_cbor_auth_key,
                                             sizeof(init_cbor_auth_key)),
                          EDHOC_SUCCESS);
    TEST_ASSERT_EQUAL_INT(edhoc_load_ephkey(&ctx, init_cbor_eph_key,
                                            sizeof(init_cbor_eph_key)), EDHOC_SUCCESS);
    TEST_ASSERT_EQUAL_INT(edhoc_session_preset_cidi(&ctx, init_cid, sizeof(init_cid)),
                          EDHOC_SUCCESS);

    cred_id_t cred_id_ctx;
    cred_id_init(&cred_id_ctx);
    cred_id_from_cbor(&cred_id_ctx, init_cbor_rpk_id, sizeof(init_cbor_rpk_id));
    rpk_t rpk_ctx;
    cred_rpk_init(&rpk_ctx);
    cred_rpk_from_cbor(&rpk_ctx, init_cbor_rpk, sizeof(init_cbor_rpk));
    edhoc_conf_setup_credentials(&conf, &authkey, CRED_TYPE_RPK, &rpk_ctx, &cred_id_ctx, _cred_cb);

    edhoc_conf_setup_role(&conf, EDHOC_IS_INITIATOR);
    struct tc_sha256_state_struct crypto_ctx;
    edhoc_ctx_setup(&ctx, &conf, &crypto_ctx);

    TEST_ASSERT_EQUAL_INT(sizeof(expected_m1),
                          edhoc_create_msg1(&ctx, CORR_1_2, EDHOC_AUTH_STATIC_STATIC,
                                            EDHOC_CIPHER_SUITE_0, msg1,
                                            sizeof(msg1)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(msg1, expected_m1, sizeof(expected_m1)));

    TEST_ASSERT_EQUAL_INT(sizeof(expected_m3),
                          edhoc_create_msg3(&ctx, msg2, sizeof(msg2), msg3, sizeof(msg3)));
    TEST_ASSERT_EQUAL_INT(0, memcmp(msg3, expected_m3, sizeof(expected_m3)));

}

TestRef tests_edhoc(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_msg3),
    };

    EMB_UNIT_TESTCALLER(EdhocTest, setUp, 0, fixtures);
    return (TestRef) & EdhocTest;
}

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_edhoc());
    TESTS_END();

    return 0;
}
