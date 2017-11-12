# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
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
from traceback import extract_tb, print_tb

import pexpect

PEXPECT_PATH = os.path.dirname(pexpect.__file__)
RIOTBASE = os.environ['RIOTBASE'] or \
           os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

def list_until(l, cond):
    return l[:([i for i, e in  enumerate(l) if cond(e)][0])]

def run(testfunc, timeout=10, echo=True, traceback=False):
    env = os.environ.copy()
    child = pexpect.spawnu("make term", env=env, timeout=timeout)

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
    except pexpect.TIMEOUT:
        timeouted_at = list_until(extract_tb(sys.exc_info()[2]),
                                  lambda frame:
                                      frame.filename.startswith(PEXPECT_PATH))[-1]
        print("Timeout in expect script at \"%s\" (%s:%d)" %
                    (timeouted_at.line,
                     os.path.relpath(os.path.abspath(timeouted_at.filename), RIOTBASE),
                     timeouted_at.lineno))
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    finally:
        print("")
        os.killpg(os.getpgid(child.pid), signal.SIGKILL)
        child.close()

    return 0
