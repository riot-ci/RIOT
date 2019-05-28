#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re
import os
import sys
import subprocess

from scapy.all import Ether, IPv6, UDP, \
                      IPv6ExtHdrFragment, \
                      sendp, srp1
from testrunner import run


EXT_HDR_NH = {
    IPv6ExtHdrFragment: 44,
  }


def pktbuf_empty(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    first_byte = child.match.group("first_byte")
    size = child.match.group("size")
    child.expect(
            r"~ unused: {} \(next: (\(nil\)|0), size: {}\) ~".format(
                first_byte, size))


def check_and_search_output(cmd, pattern, res_group, *args, **kwargs):
    output = subprocess.check_output(cmd, *args, **kwargs).decode("utf-8")
    for line in output.splitlines():
        m = re.search(pattern, line)
        if m is not None:
            return m.group(res_group)
    return None


def get_bridge(tap):
    res = check_and_search_output(
            ["bridge", "link"],
            r"{}.+master\s+(?P<master>[^\s]+)".format(tap),
            "master"
        )
    return tap if res is None else res


def get_host_lladdr(tap):
    res = check_and_search_output(
            ["ip", "addr", "show", "dev", tap, "scope", "link"],
            r"inet6 (?P<lladdr>[0-9A-Fa-f:]+)/64",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])

    child.expect(r"OK \((\d+) tests\)")     # wait for and check result of unittests
    print("." * int(child.match.group(1)), end="", flush=True)

    lladdr_src = get_host_lladdr(tap)
    child.sendline("ifconfig")
    child.expect("HWaddr: (?P<hwaddr>[A-Fa-f:0-9]+)")
    hwaddr_dst = child.match.group("hwaddr").lower()
    child.expect("(?P<lladdr>fe80::[A-Fa-f:0-9]+)")
    lladdr_dst = child.match.group("lladdr").lower()

    def run(func):
        if child.logfile == sys.stdout:
            func(child, tap, hwaddr_dst, lladdr_dst, lladdr_src)
        else:
            try:
                func(child, tap, hwaddr_dst, lladdr_dst, lladdr_src)
                print(".", end="", flush=True)
            except Exception as e:
                print("FAILED")
                raise e

    print("SUCCESS")


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=1, echo=False))
