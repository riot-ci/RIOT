/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     unittests
 * @{
 *
 * @file
 * @brief       Test cases for the SHA1PRNG pseudo random number generator
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "random.h"
#include "tests-prng_sha1prng.h"

/**
 * @brief expected sequence for seed=1. This sequence was generated running the
 *        following java program (openjdk 11.0.7) as a reference.
 *
 *~~~~
 * import java.security.SecureRandom;
 *
 * public class SHA1PRNGTEST {
 *   public static void main(String args[]) throws Exception {
 *     SecureRandom random = SecureRandom.getInstance("SHA1PRNG");
 *     random.setSeed(1);
 *     int number = 0;
 *     for (int i = 0; i < 20; i++) {
 *         number = random.nextInt();
 *         System.out.print(Integer.toUnsignedString(number) + " ");
 *     }
 *     System.out.println("");
 *   }
 * }
 *~~~~
 */
static const uint32_t seq_seed1[] =
   {2529905901, 3336014406, 1714755920, 3709666991, 1432426612,  554064022,
    1614405352,  861636861, 3689098857, 3893737371, 3138964692,  506954022,
    3469584855, 4144207589, 2031557795, 3248917850, 2384338299, 3341545824,
    2454801916, 3985646079};

static void test_prng_sha1prng_java_u32(void)
{
    uint32_t seed[2] = {0};
    seed[0]=1;

    uint32_t test32[sizeof(seq_seed1)];

    /* seed the generator with 8 bytes similar to the java reference
     * implementation
     */
    random_init_by_array(seed, sizeof(seed));

    /* request random samples */
    for (unsigned i = 0; i < sizeof(seq_seed1); i++) {
        test32[i] = random_uint32();
    }

    /* compare generator output and reference */
    TEST_ASSERT_EQUAL_INT(0, memcmp(test32, seq_seed1, sizeof(seq_seed1)));
}

Test *tests_prng_sha1prng_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_prng_sha1prng_java_u32),
    };

    EMB_UNIT_TESTCALLER(prng_sha1prng_tests, NULL, NULL, fixtures);

    return (Test *)&prng_sha1prng_tests;
}

void tests_prng_sha1prng(void)
{
    TESTS_RUN(tests_prng_sha1prng_tests());
}
