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

BENCHMARK_FUNCS = [
    "nop loop",
    r"mutex_init\(\)",
    "mutex lock/unlock",
    r"thread_flags_set\(\)",
    r"thread_flags_clear\(\)",
    "thread flags set/wait any",
    "thread flags set/wait all",
    "thread flags set/wait one",
    r"msg_try_receive\(\)",
    r"msg_avail\(\)"
]


def testfunc(child):
    child.expect_exact('Runtime of Selected Core API functions')
    for func in BENCHMARK_FUNCS:
        child.expect(r"\s+{func}:\s+\d+us\s+---\s+\d*\.*\d+us per call\s+---\s+\d+ calls per sec".format(
            func=func), timeout=TIMEOUT)
    child.expect_exact('[SUCCESS]')


if __name__ == "__main__":
    sys.exit(run(testfunc))
