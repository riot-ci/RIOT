#!/usr/bin/env bash

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#           2018 Kaspar Schleiser <kaspar@schleiser.de>
#           2018 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE="$(cd $(dirname $0)/../../..; pwd)"

if tput colors &> /dev/null && [ $(tput colors) -ge 8 ]; then
    CERROR="\e[1;31m"
    CWARN="\033[1;33m"
    CRESET="\e[0m"
else
    CERROR=
    CWARN=
    CRESET=
fi

DOXY_OUTPUT=$(make -C "${RIOTBASE}" doc 2>&1)
DOXY_ERRCODE=$?

if [ "${DOXY_ERRCODE}" -ne 0 ] ; then
    echo "'make doc' exited with non-zero code (${DOXY_ERRCODE})"
    echo "${DOXY_OUTPUT}"
    exit 2
else
    ERRORS=$(echo "${DOXY_OUTPUT}" | grep '.*warning' | sed "s#${PWD}/\([^:]*\)#\1#g")
    if [ -n "${ERRORS}" ] ; then
        echo -e "${CERROR}ERROR: Doxygen generates the following warnings:${CRESET}"
        echo "${ERRORS}"
        exit 2
    fi
fi

exclude_filter() {
    grep -v -e vendor -e examples -e tests -e "\<dist/tools\>"
}

# Check all groups are defined
ALL_RAW_DEFGROUP=$(git grep @defgroup -- '*.h' '*.c' '*.txt' | exclude_filter)
ALL_RAW_INGROUP=$(git grep '@ingroup' -- '*.h' '*.c' '*.txt' | exclude_filter)
DEFINED_GROUPS=$(echo "${ALL_RAW_DEFGROUP}" | \
                    grep -oE '@defgroup[ ]+[^ ]+' | \
                    grep -oE '[^ ]+$' | \
                    sed -e 's/\t.*$//' | \
                    sort)
DEFINED_GROUPS_UNIQUE=$(echo "${DEFINED_GROUPS}" | sort -u)

UNDEFINED_GROUPS=$( \
    for group in $(echo "${ALL_RAW_INGROUP}" | \
                    grep -oE '[^ ]+$' | sort -u); \
    do \
        echo "${DEFINED_GROUPS_UNIQUE}" | grep -xq "${group}" || echo "${group}"; \
    done \
    )

UNDEFINED_GROUPS_PRINT=$( \
    for group in ${UNDEFINED_GROUPS}; \
    do \
        echo -e "\n${CWARN}${group}${CRESET} found in:"; \
        echo "${ALL_RAW_INGROUP}" | grep "\<${group}\>$" | sort -u | \
                awk -F: '{ print "\t" $1 }'; \
    done \
    )

if [ -n "${UNDEFINED_GROUPS}" ]
then
    COUNT=$(echo "${UNDEFINED_GROUPS}" | wc -l)
    echo -ne "${CERROR}ERROR${CRESET} "
    echo -e "There are ${CWARN}${COUNT}${CRESET} undefined Doxygen groups:"
    echo "${UNDEFINED_GROUPS_PRINT}"
    exit 2
fi

# Check for groups defined multiple times:
# - remove empty spaces from the beginning of the line
# - replace spaces with ':' for easily looping on the result when printing
# - lines starting with '1:' are excluded
MULTIPLE_DEFINED_GROUPS=$(echo "${DEFINED_GROUPS}" | uniq -c | \
    sed 's/^ *//' | tr ' ' ':' | grep -v "^1:")

# Remove the group description from each line containing a group definition.
# Examples of input lines:
# boards/nucleo-l152re/doc.txt:@defgroup boards_nucleo-l152re STM32 Nucleo-L152RE
# boards/nucleo-l152re/include/periph_conf.h: * @defgroup boards_nucleo-l152re
# Expected output lines patterns:
# <filepath>: * <group name>
# <filepath>:<group name>
ALL_DEFGROUP_CLEANED=$(echo "${ALL_RAW_DEFGROUP}" | \
    awk -F@ '{ split($2, end, " "); printf("%s%s\n",$1,end[2]) }')

MULTIPLE_GROUPS_PRINT=$( \
    for group in ${MULTIPLE_DEFINED_GROUPS}; \
    do \
        _group_name=$(echo ${group} | awk -F: '{ printf ("%s\n",$2) }'); \
        _group_count=$(echo ${group} | awk -F: '{ printf ("%s\n",$1) }'); \
        echo -e "\n${CWARN}${_group_name}${CRESET} defined ${CWARN}${_group_count}${CRESET} times in:"; \
        echo "${ALL_DEFGROUP_CLEANED}" | grep "\<${_group_name}\>$" | sort -u | \
            awk -F: '{ print "\t" $1 }'; \
    done \
    )

if [ -n "${MULTIPLE_DEFINED_GROUPS}" ]
then
    COUNT=$(echo "${MULTIPLE_DEFINED_GROUPS}" | wc -l)
    # TODO: Change this to ERROR when all problems are fixed
    echo -ne "${CWARN}WARNING${CRESET} "
    echo -e "There are ${CWARN}${COUNT}${CRESET} Doxygen groups defined multiple times:"
    echo "${MULTIPLE_GROUPS_PRINT}"
    # TODO: uncomment when all problems are fixed
    # exit 2
fi
