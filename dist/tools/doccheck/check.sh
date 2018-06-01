#!/bin/sh

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#           2018 Kaspar Schleiser <kaspar@schleiser.de>
#           2018 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE=$(readlink -f "$(dirname $(realpath $0))/../../..")

ERRORS=$(make -C "${RIOTBASE}" doc 2>&1 | \
            grep '.*warning' | \
            sed "s#${PWD}/\([^:]*\)#\1#g")

if [ -n "${ERRORS}" ]
then
    echo "ERROR: Doxygen generates the following warnings:"
    echo "${ERRORS}"
    exit 2
fi

# Check all groups are defined

DEFINED_GROUPS="$(git grep defgroup | grep -v vendor | grep -oE '@defgroup[ ]+[^ ]+' | grep -oE '[^ ]+$' | sort -u)"

UNDEFINED_GROUPS=$( \
    for group in $(git grep '@ingroup' | grep -v vendor | grep -oE '[^ ]+$' | sort -u); \
    do \
        echo "$DEFINED_GROUPS" | grep -xq "$group" || echo "$group"; \
    done)

if [ -n "${UNDEFINED_GROUPS}" ]
then
    COUNT=$(echo "${UNDEFINED_GROUPS}" | wc -l)
    echo "WARNING: There are ${COUNT} undefined Doxygen groups:"
    echo "${UNDEFINED_GROUPS}"
fi

exit 0
