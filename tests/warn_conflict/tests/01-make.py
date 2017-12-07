#!/usr/bin/env python3

import os
import sys
import subprocess
import pexpect

BOARD = os.getenv('BOARD', 'stm32f4discovery')


def testfunc():
    cross_gcc = "arm-none-eabi-gcc"

    try:
        devnull = open(os.devnull)
        subprocess.Popen([cross_gcc],
                         stdout=devnull, stderr=devnull).communicate()
    except OSError as exc:
        if exc.errno == os.errno.ENOENT:
            print("ABORTING TEST: {} seems to be missing.\n".format(cross_gcc))
    else:
        child = pexpect.spawn(['make'], env=os.environ)

        try:
            if BOARD == 'stm32f4discovery':
                child.expect_exact(b'\x1b[1;33mThe following features may conflict:'
                                   b'\x1b[0m \x1b[1;32mperiph_dac periph_spi\x1b[0m')
                child.expect_exact(b'\x1b[1;33mRationale: '
                                   b'\x1b[0mOn stm32f4discovery boards there are '
                                   b'the same pins for the DAC and/or SPI_0.')
                child.expect_exact(b'\x1b[1;33mEXPECT undesired behaviour!\x1b[0m')
            else:
                child.expect_exact('\x1b[1;31mThe selected BOARD={} is not whitelisted:\x1b[0m stm32f4discovery'
                                   .format(BOARD).encode("ascii"))
        except (pexpect.TIMEOUT, pexpect.EOF):
            sys.exit(1)
        finally:
            child.close()

if __name__ == '__main__':
    testfunc()
