#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r'CHUNK_SIZE: (\d+)')
    chunk_size = int(child.match.group(1))
    child.expect(r'NUMBER_OF_TESTS: (\d+)')
    number_of_tests = int(child.match.group(1))
    for _ in range(number_of_tests):
        child.expect(r"Allocated {} Bytes at 0x[a-z0-9]+, total [a-z0-9]+"
                     .format(chunk_size))
        child.expect(r'Allocations count: (\d+)', timeout=60)
        allocations = int(child.match.group(1))
        for _ in range(allocations):
            child.expect(r"Free {} Bytes at 0x[a-z0-9]+, total [a-z0-9]+"
                         .format(chunk_size))
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
