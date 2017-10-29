#!/usr/bin/env python3

import os
import sys
import math

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


FACTORIAL_PARAM = 6


def testfunc(child):
    child.expect(u'main: parameter = {}'.format(FACTORIAL_PARAM))
    child.expect(u'pthread: parameter = {}'.format(FACTORIAL_PARAM))
    child.expect(u'pthread: factorial = {}'
                 .format(math.factorial(FACTORIAL_PARAM)))
    child.expect(u'main: factorial = {}'
                 .format(math.factorial(FACTORIAL_PARAM)))
    child.expect(u'SUCCESS')


if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
