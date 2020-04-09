#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import time
from testrunner import run


def _get_largest_timeout_difference(timeouts):
    next_timeout = min(timeouts)
    # get largest difference between all timeouts
    timeout = max(a - b for a in timeouts for b in timeouts)
    # check if smallest timeout (difference to 0) is the largest difference
    if timeout < next_timeout:
        timeout = next_timeout
    return timeout


def testfunc(child):
    timers = {}
    child.expect(r"Setting (\d+) timers:")
    timers_numof = int(child.match.group(1))
    for i in range(1, timers_numof + 1):
        child.expect(r" #(\d+) in (\d+) sec")
        assert i == int(child.match.group(1))
        timers[i] = int(child.match.group(2))
    assert timers_numof == len(timers)
    child.expect(r"now=(\d+)")
    check_time = int(time.time())
    offset = int(child.match.group(1))
    # get largest possible timeout for expects below
    timeout = _get_largest_timeout_difference(timers.values()) + 1
    for i in range(timers_numof):
        child.expect(r"#(\d):now=(\d+)", timeout=timeout)
        t = int(child.match.group(1))
        now = int(child.match.group(2))
        assert (int(time.time()) - check_time) >= timers[t]
        # convert seconds to microseconds
        expected = (timers[t] * (10**6)) + offset
        assert expected <= now


if __name__ == "__main__":
    sys.exit(run(testfunc))
