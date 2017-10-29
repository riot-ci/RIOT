#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect(u'START')

    for i in range(12):
        child.expect(u'Creating thread with arg {}'.format(i + 1))

    for i in range(12):
        child.expect(u'join thread {}'.format(i + 1))

    child.expect(u'SUCCESS')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
