#include "cryptoauthlib_test.h"

int main(void)
{
    /* Set device to ATECC508A */
    run_cmd("508");

    run_cmd("unit");

    return 0;
}