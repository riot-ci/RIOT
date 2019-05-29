#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Tell the lua interpreter running in riot to load some modules and print
# the value of a variable inside that module.

import os
import sys

MODULE_QUERIES = [
    ("m1", "a", "1"),
    ("m2", "a", "2"),
    ("c1", "X", "hello"),
    ("c2", "X", "world!"),
]


def test(child):
    # check startup message
    child.expect_exact('I am a module, hi!')

    # loop other defined commands and expected output
    for mod, attr, val in MODULE_QUERIES:
        child.sendline('print((require"{}").{})'.format(mod, attr))
        child.expect_exact(val)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(test))
