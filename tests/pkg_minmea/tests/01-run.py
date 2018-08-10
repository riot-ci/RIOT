#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect_exact('START')
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    exit(run(testfunc))
