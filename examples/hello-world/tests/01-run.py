#!/usr/bin/env python3

# TODO TEST FILE ONLY
# SHOULD NOT BE MERGED

import sys
import pexpect
from testrunner import run

def testfunc(child):
    child.expect_exact("Hello World!")

    child.sendline("the test should not match this")
    try:
        child.expect_exact("the test should not match this")
    except pexpect.TIMEOUT:
        pass
    else:
        raise RuntimeError("There should have been a timeout on the match,"
                           " and not match on stdin")


if __name__ == "__main__":
    sys.exit(run(testfunc))
