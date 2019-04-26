#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import datetime
import sys
import socket
import struct
import time
from testrunner import run
from pexpect import EOF, TIMEOUT

# ZEP v2 data header (see `sys/include/net/zep.h`)
ZEP_HDR = "!2sBBBHBBLLL10xB"
ZEP_V2_VERSION = 2
ZEP_V2_TYPE_DATA = 1

SYSTEM_EPOCH = datetime.date(*time.gmtime(0)[0:3])
NTP_EPOCH = datetime.date(1900, 1, 1)
NTP_DELTA = (SYSTEM_EPOCH - NTP_EPOCH).days * 24 * 3600

the_socket = None


def system_to_ntp_time(date):
    """convert a system time to a NTP time"""
    return date + NTP_DELTA


def testfunc(child):
    global the_socket
    assert(the_socket is not None)
    now = int(system_to_ntp_time(time.time()))
    payload = b"\x01\xdc\x00\x23\x00\x5a\x45\x00\x0a\x50\x45\x5a\x00"
    packet = struct.pack(ZEP_HDR, b"EX", ZEP_V2_VERSION,
                         ZEP_V2_TYPE_DATA, 26, 1, 1, 0xff,
                         now, 0, 1, len(payload)) + payload
    the_socket.sendto(packet, ("localhost", 17755))
    res = child.expect([TIMEOUT, EOF, "PKTDUMP: data received:"])
    # we actually want the timeout here. The application either receives a
    # bogus packet or crashes in an error case case
    assert(res == 0)


if __name__ == "__main__":
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        the_socket = s
        s.bind(("", 17754))
        sys.exit(run(testfunc, timeout=1))
