#!/usr/bin/env python3

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect(u'START')
    for i in range(4):
        child.expect(u'Start {}'.format(i + 1))
    child.expect(u'Done 2')
    child.expect(u'Done 1')
    child.expect(u'Done 3')
    child.expect(u'Done 4')
    child.expect(u'SUCCESS')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
