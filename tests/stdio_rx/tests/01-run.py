#!/usr/bin/env python3

# Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.sendline('O')
    child.expect_exact('You entered \'O\'')


if __name__ == "__main__":
    sys.exit(run(testfunc))
