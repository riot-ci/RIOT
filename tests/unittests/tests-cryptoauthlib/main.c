#include "cryptoauthlib_test.h"

int main(void)
{
    run_cmd("508"); /**< Set device to ATECC508A */

    run_cmd("info"); /**< Get revision number */
    run_cmd("sernum"); /**< Get serial nzone */
    run_cmd("lockstat"); /**< Read zone lumber */
    run_cmd("rand"); /**< Generate random numbers */
    run_cmd("readcfg"); /**< Read config ock status*/

    /* These tests will lock the config, otp and data zones, use carefully */
    #ifdef TEST_LOCKED
    run_cmd("lockcfg"); /**< Lock config zone */
    run_cmd("lockdata"); /**< Lock data zone */
    run_cmd("all"); /**< Run all unit tests, locking as needed */
    #endif

    return 0;
}