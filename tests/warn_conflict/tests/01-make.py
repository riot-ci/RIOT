#!/usr/bin/env python3

import subprocess
import os


def testfunc():
    cross_gcc = "arm-none-eabi-gcc"

    try:
        devnull = open(os.devnull)
        subprocess.Popen([cross_gcc],
                         stdout=devnull, stderr=devnull).communicate()
        output = subprocess.Popen(["make", "BOARD=stm32f4discovery"],
                                  stderr=subprocess.PIPE,
                                  stdout=subprocess.PIPE)
        _, err = output.communicate()

        compare = (b'\x1b[1;33mThe following features may conflict:\x1b[0m '
                   b'\x1b[1;32mperiph_dac periph_spi\x1b[0m\n'
                   b'\x1b[1;33mRationale: '
                   b'\x1b[0mOn stm32f4discovery boards there are the same pins for the DAC and/or SPI_0.\n\n'
                   b'\x1b[1;33mEXPECT undesired behaviour!\x1b[0m\n')
        if compare in err:
            print("Test SUCCEEDED! Compile time output is as expected!\n")
        else:
            print("Test FAILED! Compile time output is NOT as expected!\n")
            print("Expected:\n{}".format(compare.decode("ascii")))
            print("Received:\n{}".format(err.decode("ascii")))

    except OSError as exc:
        if exc.errno == os.errno.ENOENT:
            print("ABORTING TEST: {} seems to be missing.\n".format(cross_gcc))

if __name__ == '__main__':
    testfunc()
