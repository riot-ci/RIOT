#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2020 Martine Lenders <m.lenders@fu-berlin.de>
#
# Distributed under terms of the MIT license.


def split_prefix(route):
    comp = route.split("/")
    return comp[0], int(comp[1])
