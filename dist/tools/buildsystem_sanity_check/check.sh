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

SCRIPT_PATH=dist/tools/buildsystem_sanity_check/check.sh

# Modules should not check the content of FEATURES_PROVIDED/_REQUIRED/OPTIONAL
# Handling specific behaviors/dependencies should by checking the content of:
# * `USEMODULE`
# * maybe `FEATURES_USED` if it is not a module (== not a periph_)
check_not_parsing_features() {
    local patterns=()
    local pathspec=()

    patterns+=(-e 'if.*filter.*FEATURES_PROVIDED')
    patterns+=(-e 'if.*filter.*FEATURES_REQUIRED')
    patterns+=(-e 'if.*filter.*FEATURES_OPTIONAL')

    # Pathspec with exclude should start by an inclusive pathspec in git 2.7.4
    pathspec+=('*')

    # Ignore this file when matching as it self matches
    pathspec+=(":!${SCRIPT_PATH}")

    # These two files contain sanity checks using FEATURES_ so are allowed
    pathspec+=(':!Makefile.include' ':!makefiles/info-global.inc.mk')

    git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}"
}

# Makefile.features for cpu/ must not be included by the board anymore
# They are included by the main Makefile.features
check_board_do_not_include_cpu_features() {
    local patterns=()
    local pathspec=()

    # shellcheck disable=SC2016
    # Single quotes are used to not expand expressions
    patterns+=(-e 'include $(RIOTCPU)/.*/Makefile.features')

    pathspec+=('boards/')

    git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}"
}

# CPU and CPU_MODEL definition have been moved to 'BOARD/Makefile.features'
check_cpu_cpu_model_defined_in_makefile_features() {
    local patterns=()
    local pathspec=()

    # With our without space and with or without ?=
    patterns+=(-e '^ *\(export\)\? *CPU \??\?=')
    patterns+=(-e '^ *\(export\)\? *CPU_MODEL \??\?=')
    pathspec+=(':!boards/**/Makefile.features') # ':!makefiles/info-global.inc.mk')

    # Ignore this file when matching as it self matches
    pathspec+=(":!${SCRIPT_PATH}")
    git -C "${RIOTBASE}" grep "${patterns[@]}" -- "${pathspec[@]}"
}


main() {
    local errors=''

    errors+="$(check_not_parsing_features)"
    errors+="$(check_board_do_not_include_cpu_features)"
    errors+="$(check_cpu_cpu_model_defined_in_makefile_features)"

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
