#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


NUMBER_OF_TESTS = int(os.getenv("NUMBER_OF_TESTS", 12))
MAX_NUMBER_BLOCKS = int(os.getenv("MAX_NUMBER_BLOCKS", 10))


def testfunc(child):
    for test in range(NUMBER_OF_TESTS):
        child.expect_exact("TEST #{}:".format(test + 1))
        for i in range(MAX_NUMBER_BLOCKS):
            child.expect(r"\({}, @@@@@@@\) Allocated 16 Bytes at 0x[a-z0-9]+, total [a-z0-9]+"
                         .format(i))
        for i in range(MAX_NUMBER_BLOCKS):
            child.expect(r"Free \({}\) 16 Bytes at 0x[a-z0-9]+, total [a-z0-9]+"
                         .format(i))
    child.expect_exact("Finishing")


if __name__ == "__main__":
    sys.exit(run(testfunc))
