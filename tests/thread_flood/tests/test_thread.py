#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

def testfunc(term):
    term.expect(r'\[START\] .*\r\n')
    term.expect(r'\.*\r\n')
    term.expect(r'\[SUCCESS\] created \d+')

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
