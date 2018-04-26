#!/usr/bin/env python3

# Copyright (C) 2017 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

# It takes 32 seconds on wsn430, so add some margin
TIMEOUT = 45


def testfunc(child):
    for i in range(20):
        child.expect(r"\[ALIVE\] alternated \d+k times.", timeout=TIMEOUT)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
