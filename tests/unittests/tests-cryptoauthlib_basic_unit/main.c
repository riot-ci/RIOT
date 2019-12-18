#include "cryptoauthlib_test.h"

int main(void)
{
    run_cmd("508"); /**< Set device to ATECC508A */
    
    run_cmd("unit"); /**< Run unit test on selected device */
    run_cmd("util"); /**< Run Helper Function Tests */

    /* These tests will lock the config, otp and data zones, use carefully */
    #ifdef TEST_LOCKED
    run_cmd("basic"); /**< Run basic test on selected device */
    #endif

    return 0;
}