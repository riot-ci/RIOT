#!/usr/bin/env python3

import os
import sys

NB_TESTS = 3


def testfunc(child):
    for test in range(1, NB_TESTS + 1):
        child.expect_exact('Test {}:'.format(test))
        child.expect('Buffer: [0-9A-Z]')
        child.expect('Result: SUCCESS')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
