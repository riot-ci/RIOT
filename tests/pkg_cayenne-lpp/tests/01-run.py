#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact('03670110056700FF')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
