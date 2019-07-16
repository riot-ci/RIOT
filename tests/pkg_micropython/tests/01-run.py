#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('boot.py: MicroPython says hello!')
    child.expect_exact('>>>')

    child.sendline('print("echo this! " * 4)')
    child.expect_exact('echo this! echo this! echo this! echo this!')
    child.expect_exact('>>>')
    print("[TEST PASSED]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
