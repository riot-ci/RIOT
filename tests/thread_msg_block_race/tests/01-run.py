#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner.unittest import PexpectTestCase
from pexpect import TIMEOUT
import unittest


class TestMessageNotWritten(PexpectTestCase):
    def test_message_not_written(self):
        with self.assertRaises(TIMEOUT):
            # we actually want the timeout here. The application runs into an
            # assertion pretty quickly when failing and runs forever on success
            self.spawn.expect("Message was not written")


if __name__ == "__main__":
    unittest.main()
