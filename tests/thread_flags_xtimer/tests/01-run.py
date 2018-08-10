#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect("START")
    for i in range(5):
        child.expect_exact("+++ timeout  {} +++".format(i + 1))
    child.expect("SUCCESS")


if __name__ == "__main__":
    exit(run(testfunc))
