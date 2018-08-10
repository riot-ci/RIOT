#!/usr/bin/env python3

from testrunner import run


def testfunc(child):
    child.expect_exact(u'[START] Spawning threads')
    child.expect(r'\.+')
    child.expect(r'\[SUCCESS\] created \d+')


if __name__ == "__main__":
    exit(run(testfunc))
