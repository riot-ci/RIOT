#!/usr/bin/env python3

import os
import sys


def testfunc(child):
    child.expect_exact('03670110056700FF')
    # floating point precision may vary between architectures (native, boards)
    child.expect('0167FFD[78]067104D[12]FB2[EF]0000')
    child.expect_exact('018806765EF2960A0003E8')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
