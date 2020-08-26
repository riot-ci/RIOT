#!/usr/bin/env python3

import sys
from testrunner import run


TIMEOUT = 60

def testfunc(child):
    child.expect_exact("Test complete")
    child.expect_exact("Benchmark complete",timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
