#include "cryptoauthlib_test.h"

int main(void)
{
    run_cmd("508"); /**< Set device to ATECC508A */

    run_cmd("cd"); /**< Run Unit Tests on Cert Data */
    run_cmd("cio"); /**< Run Unit Test on Cert I/O */

    return 0;
}