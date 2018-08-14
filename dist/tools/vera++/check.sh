#!/bin/bash

# Copyright 2018 Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd "$(dirname "$0")" && pwd)
: "${VERA_PROFILE:=riot}"
: "${QUIET:=0}"
: "${WARNONLY:=0}"

PARAMS=""
if [ $QUIET -eq 1 ]; then
    PARAMS="$PARAMS --quiet"
fi
if [ $WARNONLY -eq 0 ]; then
    PARAMS="$PARAMS --error"
fi

. "$RIOTBASE"/dist/tools/ci/changed_files.sh
sed -e "s#^#$RIOTBASE/#" <(changed_files) | vera++ --root "$CURDIR" \
    --profile $VERA_PROFILE --exclusions "$CURDIR"/exclude $PARAMS

