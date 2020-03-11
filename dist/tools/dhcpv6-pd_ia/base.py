# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


import shutil
import subprocess
import threading

import pkg


__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"


# see https://refactoring.guru/design-patterns/singleton/python/example
class _SingletonMeta(type):
    """
    This is a thread-safe implementation of Singleton.
    """

    _instance = None

    _lock = threading.Lock()
    """
    We now have a lock object that will be used to synchronize threads
    during first access to the Singleton.
    """

    def __call__(cls, *args, **kwargs):
        # Now, imagine that the program has just been launched. Since
        # there's no Singleton instance yet, multiple threads can
        # simultaneously pass the previous conditional and reach this point
        # almost at the same time. The first of them will acquire lock and
        # will proceed further, while the rest will wait here.
        with cls._lock:
            # The first thread to acquire the lock, reaches this
            # conditional, goes inside and creates the Singleton instance.
            # Once it leaves the lock block, a thread that might have been
            # waiting for the lock release may then enter this section. But
            # since the Singleton field is already initialized, the thread
            # won't create a new object.
            if not cls._instance:
                cls._instance = super().__call__(*args, **kwargs)
        return cls._instance


class DHCPv6Server(metaclass=_SingletonMeta):
    PORT = 547
    command = None
    installer = None

    def __init__(self):
        raise NotImplementedError("Please create a class inheriting from this")

    @classmethod
    def is_installed(cls):
        return shutil.which(cls.command[0]) is not None

    def install(self):
        self.installer = pkg.PackageManagerFactory.get_installer()
        self.installer.install(self.package)

    def run(self):
        if not self.is_installed():
            self.install()
        subprocess.run(self.command)
