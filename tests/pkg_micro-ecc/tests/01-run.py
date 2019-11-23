#!/usr/bin/env python3

import sys
from testrunner import run


# Use a custom global timeout for slow hardware. On ATmegas clocked at 8MHz
# test completes in ~10 min
TIMEOUT = 600


def testfunc(child):
    child.expect_exact('micro-ecc compiled!')
    child.expect_exact('Testing 16 random private key pairs and signature '
                       'without using HWRNG')
    for i in range(15):
        child.expect_exact("Round {}".format(i))
    child.expect_exact('Done with 0 error(s)')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT))
