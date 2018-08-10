#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect('START')
    child.expect('SUCCESS')


if __name__ == "__main__":
    exit(run(testfunc))
