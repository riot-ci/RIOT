#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect_exact('If you can read this:')
    child.expect_exact('Test successful.')


if __name__ == "__main__":
    exit(run(testfunc))
