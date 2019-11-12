#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r'MAX_NUMBER_BLOCKS: (\d+)')
    max_number_blocks = int(child.match.group(1))
    child.expect(r'NUMBER_OF_TESTS: (\d+)')
    number_of_tests = int(child.match.group(1))
    for test in range(number_of_tests):
        child.expect_exact("TEST #{}:".format(test + 1))
        for i in range(max_number_blocks):
            child.expect(r"\({}, @@@@@@@\) Allocated 16 Bytes at 0x[a-z0-9]+, total [a-z0-9]+"
                         .format(i))
        for i in range(max_number_blocks):
            child.expect(r"Free \({}\) 16 Bytes at 0x[a-z0-9]+, total [a-z0-9]+"
                         .format(i))
    child.expect_exact("Finishing")


if __name__ == "__main__":
    sys.exit(run(testfunc))
