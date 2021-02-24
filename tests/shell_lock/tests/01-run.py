#!/usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys, time, os
from testrunner import run

PASSWORD_CORRECT = "password"
PASSWORDS_INCORRECT = [
    "pass",
    "word",
    "asswor",
    "passw0rd",
    "password_",
    "_password"
]

EXPECTED_HELP = (
    'Command              Description',
    '---------------------------------------',
    'reboot               Reboot the node',
    'version              Prints current RIOT_VERSION',
    'pm                   interact with layered PM subsystem',
    'lock                 Lock the shell'
)

AUTO_LOCK_TIMEOUT_MS = 7000
PROMPT = '> '
BOARD = os.environ['BOARD']

def testfunc(child):

    # avoid sending an extra empty line on native.
    if BOARD == 'native':
        child.crlf = '\n'

    # unlock
    child.sendline(PASSWORD_CORRECT)
    child.expect_exact('Shell was unlocked.')
    child.expect_exact(PROMPT)

    # check we have access
    child.sendline('help')
    for line in EXPECTED_HELP:
        child.expect_exact(line)

    # lock
    child.sendline('lock')
    child.expect(PROMPT)

    # trigger password prompt
    child.sendline('help')
    child.expect('The shell is locked. Enter a valid password to unlock.')
    child.expect('Password:')

    # test different incorrect passwords
    for i, pwd in enumerate(PASSWORDS_INCORRECT):

        # every third incorrect attempt leads to 7 second of sleep, otherwise
        # just 1 second
        if i > 0 and i % 3 == 0:
            timeout = 7
        else:
            timeout = 1

        # some boards react quite slow, give them 2 extra seconds
        child.expect_exact(PROMPT, timeout=(timeout + 2))
        child.sendline(pwd)
        child.expect_exact('Wrong password')

    # unlock
    child.sendline(PASSWORD_CORRECT)
    child.expect_exact('Shell was unlocked.')
    child.expect_exact(PROMPT)

    # check we have access
    child.sendline('help')
    for line in EXPECTED_HELP:
        child.expect_exact(line)

    # wait until auto_lock locks the shell after AUTO_LOCK_TIMEOUT_MS
    time.sleep((AUTO_LOCK_TIMEOUT_MS / 1000.0) + 1)

    # trigger password prompt
    child.sendline('help')
    child.expect('The shell is locked. Enter a valid password to unlock.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
