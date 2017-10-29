#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect(u'START')
    for _ in range(8):
        child.expect(u'start')

    for _ in range(8):
        child.expect(u'done')

    child.expect(u'SUCCESS')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
