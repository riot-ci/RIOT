#include "cryptoauthlib_test.h"

int main(void)
{
    run_cmd("508"); /**< Set device to ATECC508A */

    run_cmd("crypto"); /**< Run Unit Tests for Software Crypto Functions */

    return 0;
}