#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


# The default timeout is not enough for this test on some of the slower boards
TIMEOUT = 30


def testfunc(child):
    child.expect_exact('[SUCCESS]', timeout=TIMEOUT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
