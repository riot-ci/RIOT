#!/usr/bin/env python3

# Copyright (C) 2020 SSV Software Systems GmbH
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from __future__ import print_function
import sys
from testrunner import run

how_many = 1000


def testfunc(child):
    for i in range(how_many):
        child.expect(r'Loop %i' % (i))
        print(".", end="", flush=True)
    print("")
    print("Stopped after %i iterations, but should run forever." % how_many)
    print("=> All tests successful")

if __name__ == "__main__":
    sys.exit(run(testfunc))
