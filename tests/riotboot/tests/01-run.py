#!/usr/bin/env python3

# Copyright (C) 2018 Federico Pellegrin <fede@evolware.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import subprocess
from testrunner import run


def flash_slot(slotnum, version):
    cmd = [
        "make",
        "RIOTBOOT_SKIP_COMPILE=1",
        "riotboot/flash-slot{}".format(slotnum),
        "APP_VER={}".format(version),
    ]
    assert not subprocess.call(cmd)


def assert_slot_num(child, slotnum, version):
    # Check if its runing on the expected slot
    child.sendline("curslotnr")
    child.expect("Current slot=%s" % (slotnum))
    child.expect('>')

    # Ask for current slot header info and checks for basic output integrity
    child.sendline("curslothdr")
    # Magic number is "RIOT" (always in little endian)
    child.expect_exact("Image magic_number: 0x544f4952")
    # Other info is hardware/app dependant so we just check basic compliance
    child.expect("Image Version: %s" % ("{0:#0{1}x}".format(version, 10)))
    child.expect("Image start address: 0x[0-9a-fA-F]{8}")
    child.expect("Header chksum: 0x[0-9a-fA-F]{8}")
    child.expect('>')

    # Ask for address of slot 0
    child.sendline("getslotaddr 0")
    child.expect("Slot 0 address=0x[0-9a-fA-F]{8}")
    child.expect('>')

    # Ask for data of all slots
    child.sendline("dumpaddrs")
    child.expect("slot 0: metadata: 0x[0-9a-fA-F]{1,8} image: 0x[0-9a-fA-F]{8}")
    child.expect('>')


def testfunc(child):

    # Ask for current slot, should be 0 or 1
    child.sendline("curslotnr")
    child.expect(r"Current slot=([0-1])")
    slotnum = int(child.match.group(1))
    child.expect('>')

    # Ask for current APP_VER
    child.sendline("curslothdr")
    child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F]{8}$)")
    current_app_ver = int(child.match.group("app_ver"), 16)
    child.expect('>')

    # Flash to both slots and verify basic functions
    for version in [current_app_ver + 1, current_app_ver + 2]:
        slotnum = slotnum ^ 1
        flash_slot(slotnum, version)
        assert_slot_num(child, slotnum, version)

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc))
