#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

EXPECTED_STDOUT = (
    '00|                                                |',
    '02|                                                |',
    '04|                                                |',
    '06|                                                |',
    '08|                                                |',
    '10|      █████▐▋  ▐▋▐▋ ▟██▖                        |',
    '12|        █  ▐▋  ▐▋▐▋▐▛ ▝█                        |',
    '14|        █  ▐████▋▐▋▝█▙▄                         |',
    '16|        █  ▐▋  ▐▋▐▋  ▀▜▙                        |',
    '18|        █  ▐▋  ▐▋▐▋▐▙ ▗█                        |',
    '20|        █  ▐▋  ▐▋▐▋ ▜██▘                        |',
    '22|                                                |',
    '24|                                                |',
    '26|                                                |',
    '28|                                                |',
    '30|                                                |',
    '32|                                                |',
    '34|                                                |',
    '36|                                                |',
    '38|                                                |',
    '40|                                                |',
    '42|                                                |',
    '44|                                                |',
    '46|                                                |',
    '48|                                                |',
    '50|                                                |',
    '52|                                                |',
    '54|                                                |',
    '56|                                                |',
    '58|                                                |',
    '60|                                                |',
    '62|                                                |',
    '64|                                                |',
    '66|                                                |',
    '00|                                                |',
    '02|                                                |',
    '04|                                                |',
    '06|                                                |',
    '08|                                                |',
    '10|            ▐▋ ▟██▖                             |',
    '12|            ▐▋▐▛ ▝█                             |',
    '14|            ▐▋▝█▙▄                              |',
    '16|            ▐▋  ▀▜▙                             |',
    '18|            ▐▋▐▙ ▗█                             |',
    '20|            ▐▋ ▜██▘                             |',
    '22|                                                |',
    '24|                                                |',
    '26|                                                |',
    '28|                                                |',
    '30|                                                |',
    '32|                                                |',
    '34|                                                |',
    '36|                                                |',
    '38|                                                |',
    '40|                                                |',
    '42|                                                |',
    '44|                                                |',
    '46|                                                |',
    '48|                                                |',
    '50|                                                |',
    '52|                                                |',
    '54|                                                |',
    '56|                                                |',
    '58|                                                |',
    '60|                                                |',
    '62|                                                |',
    '64|                                                |',
    '66|                                                |',
    '00|           ▄▄▖                                  |',
    '02|         ▗████▋                                 |',
    '04|        ▗█▛▘ ▜█▖                                |',
    '06|        ▐█    █▋                                |',
    '08|        ██    █▋                                |',
    '10|        ██   ▟█▘                                |',
    '12|        ▜█ ▄▟█▛  ▄   ▄▄  ▄▄▄▄                   |',
    '14|    ▗▄█▖▐█▝██▀   █▋ ▟▛▜▙ ▀▜▛▀                   |',
    '16|   ▗██▀ ▐█ ▝     █▋ █▋ █  ▐▋                    |',
    '18|   ▟▛   ▐█  ▄    █▋ █▋ █  ▐▋                    |',
    '20|   █▋   ▐█ ▝█▋   █▋ █▋ █  ▐▋                    |',
    '22|   █▋   ▐█  ▜█   █▋ █▋ █  ▐▋                    |',
    '24|   █▙  ▗█▛  ▝█▙  █▋ █▋▗█  ▐▋                    |',
    '26|   ▐█▙▄██▘   ▜█  █▘ ▝██▘  ▐▋                    |',
    '28|    ▝▜█▀▘                                       |',
    '30|                                                |',
    '32|                                                |',
    '34|                                                |',
    '36|                                                |',
    '38|                                                |',
    '40|                                                |',
    '42|                                                |',
    '44|                                                |',
    '46|                                                |',
    '48|                                                |',
    '50|                                                |',
    '52|                                                |',
    '54|                                                |',
    '56|                                                |',
    '58|                                                |',
    '60|                                                |',
    '62|                                                |',
    '64|                                                |',
    '66|                                                |',
)


def testfunc(child):
    child.expect_exact('Initializing to stdout.')
    child.expect_exact('Initializing display.')
    child.expect_exact('Drawing on screen.')

    for line in EXPECTED_STDOUT:
        child.expect_exact(line)
    import os
    # Make it fail for one of the cases
    if os.environ.get('TOOLCHAIN', 'llvm'):
        assert(0)


if __name__ == "__main__":
    sys.exit(run(testfunc))
