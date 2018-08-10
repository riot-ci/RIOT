#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect_exact('SUCCESS: Libcoap compiled!')


if __name__ == "__main__":
    exit(run(testfunc))
