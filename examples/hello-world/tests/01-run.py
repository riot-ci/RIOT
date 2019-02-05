#!/usr/bin/env python3

# TODO TEST FILE ONLY
# SHOULD NOT BE MERGED

import sys
import pexpect
from testrunner import run

def testfunc(child):
    child.expect_exact("Hello World!")

    msg = "the test should not match this"
    child.sendline(msg)
    res = child.expect_exact([pexpect.TIMEOUT, msg])
    assert res == 0, "There should have been a timeout and not match stdin"


if __name__ == "__main__":
    sys.exit(run(testfunc))
