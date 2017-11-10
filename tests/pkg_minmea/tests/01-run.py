#!/usr/bin/env python3

import os
import sys
import math

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner


def testfunc(child):
    child.expect('START')
    child.expect_exact('parsed coordinates: lat=52.483630 lon=13.446008')
    child.expect('SUCCESS')

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
