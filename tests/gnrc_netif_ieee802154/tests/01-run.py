#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import socket
import struct
from testrunner import run
from pexpect import EOF, TIMEOUT

# ZEP v2 data header (see `sys/include/net/zep.h`)
ZEP_HDR = "!2sBBBHBBLLL10xB"
ZEP_V2_VERSION = 2
ZEP_V2_TYPE_DATA = 1


def testfunc(child):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind(("", 17754))
        # first send valid packet to check if communication is set up correctly
        # generated with udp send <link-local> 61616 0 in tests/gnrc_udp
        payload = b"\x61\xdc\x85\x23\x00\x5a\x45\x00\x0a\x50\x45\x5a\x00\x2a\xab\xdc" \
                  b"\x15\x54\x01\x64\x79\x7e\x33\xf3\x00\xa8\x80"
        packet = struct.pack(ZEP_HDR, b"EX", ZEP_V2_VERSION,
                             ZEP_V2_TYPE_DATA, 26, 1, 1, 0xff,
                             0, 0, 1, len(payload)) + payload
        s.sendto(packet, ("localhost", 17755))
        child.expect("PKTDUMP: data received:")
        child.expect("00000000  7E  33  F3  00")
        child.expect("~~ PKT    -  2 snips, total size:  28 byte")
        payload = b"\x01\xdc\x00\x23\x00\x5a\x45\x00\x0a\x50\x45\x5a\x00"
        # child.expect("PKTDUMP: data received:")
        packet = struct.pack(ZEP_HDR, b"EX", ZEP_V2_VERSION,
                             ZEP_V2_TYPE_DATA, 26, 1, 1, 0xff,
                             0, 0, 2, len(payload)) + payload
        s.sendto(packet, ("localhost", 17755))
        res = child.expect([TIMEOUT, EOF, "PKTDUMP: data received:"])
        # we actually want the timeout here. The application either
        # receives a bogus packet or crashes in an error case case
        assert(res == 0)


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1))
