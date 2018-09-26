#!/usr/bin/env bash
#
# Copyright (C) 2018 Gaëtan Harter <gaetan.harter@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

#
# Central test script to have sanity checks for the build system
# It is run unconditionally on all files.
#
#

: "${RIOTBASE:="$(cd "$(dirname "$0")/../../../" || exit; pwd)"}"

# Ignore this file when matching as it self matches)
PATHSPEC=(':!dist/tools/buildsystem_sanity_check/check.sh')

# Modules should not check the content of FEATURES_PROVIDED/_REQUIRED/OPTIONAL
# Handling specific behaviors/dependencies should by checking the content of:
# * `USEMODULE`
# * maybe `FEATURES_USED` if it is not a module (== not a periph_)
check_not_parsing_features() {
    local patterns=()
    local pathspec=("${PATHSPEC[@]}")

    patterns+=(-e 'if.*filter.*FEATURES_PROVIDED')
    patterns+=(-e 'if.*filter.*FEATURES_REQUIRED')
    patterns+=(-e 'if.*filter.*FEATURES_OPTIONAL')

    # These two files contain sanity checks using FEATURES_ so are allowed
    pathspec+=(':!Makefile.include' ':!makefiles/info-global.inc.mk')

    # TODO remove when dependencies is resolved before Makefile.include
    # It requires parsing 'cpu' dependencies which is not the case now.
    pathspec+=(':!cpu/stm32_common/Makefile.include')

    git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}"
}


main() {
    local errors=''

    errors+="$(check_not_parsing_features)"

    if [ -n "${errors}" ]
    then
        printf 'Invalid build system patterns found by %s:\n' "${0}"
        printf '%s\n' "${errors}"
        exit 1
    fi
    exit 0
}


if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main
fi
