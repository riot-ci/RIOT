#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"Iface\s+\d+")
    child.expect(r"inet6 addr:\sfe80:[0-9a-f:]+\s+scope: link")
    child.expect(r"Iface\s+\d+")
    child.expect(r"inet6 addr:\s+fe80:[0-9a-f:]+\s+scope: link")
    child.expect(r"inet6 addr:\s+(?P<global_addr>[0-9a-f:]+)\s+scope: global")
    global_addr = child.match.group("global_addr")
    child.expect(r"(?P<global_pfx>[0-9a-f:]+)/64\s+dev #\d\s+"
                 r"expires (?P<valid_ltime>\d+)sec\s+"
                 r"deprecates (?P<pref_ltime>\d+)sec")
    global_pfx = child.match.group("global_pfx")
    assert int(child.match.group("valid_ltime")) < 4000
    assert int(child.match.group("pref_ltime")) < 3000
    if global_pfx.endswith("::"):
        # remove one trailing : in case there are no 0s between prefix and
        # suffix
        global_pfx = global_pfx[0:-1]
    assert global_addr.startswith(global_pfx)


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=5))
