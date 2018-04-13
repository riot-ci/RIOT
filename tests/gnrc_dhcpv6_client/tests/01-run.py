#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


import os
import sys


def testfunc(child):
    child.expect(r"Iface\s+\d\s+HWaddr: [0-9a-f:]+")
    child.expect(r"inet6 addr:\sfe80:[0-9a-f:]+\s+scope: local")
    child.expect(r"Iface\s+\d\s+HWaddr: [0-9a-f:]+")
    child.expect(r"inet6 addr:\s+fe80:[0-9a-f:]+\s+scope: local")
    child.expect(r"inet6 addr:\s+[0-9a-f:]+\s+scope: global")


if __name__ == "__main__":
    sys.path.append(
            os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner')
        )
    from testrunner import run
    status = run(testfunc, timeout=5)
