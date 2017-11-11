# Copyright (C) 2017 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
#               2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import signal
import sys
import subprocess
import time
from traceback import print_tb

from pexpect import spawnu, TIMEOUT


def run(testfunc, timeout=10, echo=True, traceback=False):
    env = os.environ.copy()
    child = spawnu("make term", env=env, timeout=timeout)

    # on many platforms, the termprog needs a short while to be ready...
    time.sleep(3)

    if echo:
        child.logfile = sys.stdout

    try:
        subprocess.check_output(('make', 'reset'), env=env,
                                stderr=subprocess.PIPE)
    except subprocess.CalledProcessError:
        # make reset yields error on some boards even if successful
        pass
    try:
        testfunc(child)
    except TIMEOUT:
        print("Timeout in expect script")
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    except EOF:
        print("EOF in expect script")
        return 1
    finally:
        print("")
        try:
            os.killpg(os.getpgid(child.pid), signal.SIGKILL)
        except ProcessLookupError:
            print("Process already stopped")

        child.close()
    return 0
