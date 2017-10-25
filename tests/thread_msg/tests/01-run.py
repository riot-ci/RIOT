#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect(u'THREADS CREATED')
    child.expect(u'THREAD 1 start')
    child.expect(u'THREAD 2 start')
    child.expect(u'THREAD 3 start')
    child.expect(u'THREAD 1 end')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
