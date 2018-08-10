#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect('main starting')
    child.expect('msg available: 1 \(should be 1\!\)')


if __name__ == "__main__":
    exit(run(testfunc))
