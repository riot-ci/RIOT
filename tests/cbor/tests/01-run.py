#!/usr/bin/env python3

# Copyright (C) 2017 OTA keys S.A.
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

ACCEPTED_ERROR = 20

def testfunc(child):
    child.expect(r'''Data:\r
\(uint64_t, 1\)\r
\(uint64_t, 2\)\r
\(uint64_t, 3\)\r
\(int64_t, -5\)\r
\(bool, 1\)\r
\(float, 1\.099609\)\r
\(float, 1\.500000\)\r
\(double, 2\.000000\)\r
\(byte string, "abc"\)\r
\(unicode string, "def"\)\r
\(array, length: 2\)\r
  \(uint64_t, 0\)\r
  \(uint64_t, 1\)\r
\(array, length: \[indefinite\]\)\r
  \(uint64_t, 10\)\r
  \(uint64_t, 11\)\r
\(map, length: 2\)\r
 \(uint64_t, 1\)\r
  \(byte string, "1"\)\r
 \(uint64_t, 2\)\r
  \(byte string, "2"\)\r
\(map, length: \[indefinite\]\)\r
 \(uint64_t, 10\)\r
  \(byte string, "10"\)\r
 \(uint64_t, 11\)\r
  \(byte string, "11"\)\r
\(tag: 0, date/time string: "[\w :]+"\)\r
\(tag: 1, date/time epoch: \d+\)\r
\(unsupported, 0xC2\r
\)\r
\(byte string, \"1\"\)''')

    print("All tests successful")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc, echo=False))
