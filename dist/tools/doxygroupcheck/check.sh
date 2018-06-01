#!/bin/sh

# Copyright 2018 Kaspar Schleiser
#                Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

DEFINED_GROUPS="$(git grep defgroup | grep -v vendor | grep -oE '@defgroup[ ]+[^ ]+' | grep -oE '[^ ]+$' | sort -u)"

UNDEFINED_GROUPS=$( \
    for group in $(git grep '@ingroup' | grep -v vendor | grep -oE '[^ ]+$' | sort -u); \
    do \
        echo "$DEFINED_GROUPS" | grep -xq "$group" || echo "$group"; \
    done)

if [ -n "${UNDEFINED_GROUPS}" ]
then
    echo "ERROR: There are undefined Doxygen groups:"
    echo "${UNDEFINED_GROUPS}"
    exit 1
else
    exit 0
fi
