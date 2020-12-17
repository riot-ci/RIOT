#!/usr/bin/env bash

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#           2018 Kaspar Schleiser <kaspar@schleiser.de>
#           2018 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE="$(cd $(dirname $0)/../../..; pwd)"

. ${RIOTBASE}/dist/tools/ci/github_annotate.sh

github_annotate_setup

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
RESULT=0


if [ "${DOXY_ERRCODE}" -ne 0 ] ; then
    echo "'make doc' exited with non-zero code (${DOXY_ERRCODE})"
    echo "${DOXY_OUTPUT}"
    RESULT=2
else
    ERRORS=$(echo "${DOXY_OUTPUT}" | grep '.*warning' | sed "s#${PWD}/\([^:]*\)#\1#g")
    if [ -n "${ERRORS}" ] ; then
        echo -e "${CERROR}ERROR: Doxygen generates the following warnings:${CRESET}"
        echo "${ERRORS}"
        echo "${ERRORS}" | grep "^.\+:[0-9]\+: warning:" | while read error_line
        do
            FILENAME=$(echo "${error_line}" | cut -d: -f1)
            LINENR=$(echo "${error_line}" | cut -d: -f2)
            DETAILS=$(echo "${error_line}" | cut -d: -f4- |
                      sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')
            github_annotate_error "${FILENAME}" "${LINENR}" "${DETAILS}"
        done
        RESULT=2
    fi
fi

exclude_filter() {
    grep -v -e vendor -e examples -e tests -e "\<dist/tools\>"
}

# Check groups are correctly defined (e.g. no undefined groups and no group
# defined multiple times)
ALL_RAW_DEFGROUP=$(git grep -n @defgroup -- '*.h' '*.c' '*.txt' | exclude_filter)
ALL_RAW_INGROUP=$(git grep -n '@ingroup' -- '*.h' '*.c' '*.txt' | exclude_filter)
DEFINED_GROUPS=$(echo "${ALL_RAW_DEFGROUP}" | \
                    grep -oE '@defgroup[ ]+[^ ]+' | \
                    grep -oE '[^ ]+$' | \
                    sort)
DEFINED_GROUPS_UNIQUE=$(echo "${DEFINED_GROUPS}" | sort -u)

# Check for undefined groups
UNDEFINED_GROUPS=$( \
    for group in $(echo "${ALL_RAW_INGROUP}" | \
                    grep -oE '[^ ]+$' | sort -u); \
    do \
        echo "${DEFINED_GROUPS_UNIQUE}" | grep -xq "${group}" || echo "${group}"; \
    done \
    )

if [ -n "${UNDEFINED_GROUPS}" ]
then
    COUNT=$(echo "${UNDEFINED_GROUPS}" | wc -l)
    echo -ne "\n\n${CERROR}ERROR${CRESET} "
    echo -e "There are ${CWARN}${COUNT}${CRESET} undefined Doxygen groups:"
    for group in ${UNDEFINED_GROUPS};
    do
        INGROUPS=$(echo "${ALL_RAW_INGROUP}" | grep "\<${group}\>$" | sort -u)
        echo -e "\n${CWARN}${group}${CRESET} found in:";
        echo "${INGROUPS}" | awk -F: '{ print "\t" $1 }';
        echo "${INGROUPS}" | while read ingroup;
        do
            github_annotate_error $(echo ${ingroup} | awk -F: '{ print $1,$2 }') \
                "Undefined doxygen group '${group}'"
        done
    done
    RESULT=2
fi

# Check for groups defined multiple times:
MULTIPLE_DEFINED_GROUPS=$(echo "${DEFINED_GROUPS}" | uniq -d)

if [ -n "${MULTIPLE_DEFINED_GROUPS}" ]
then
    COUNT=$(echo "${MULTIPLE_DEFINED_GROUPS}" | wc -l)
    echo -ne "\n\n${CERROR}ERROR${CRESET} "
    echo -e "There are ${CWARN}${COUNT}${CRESET} Doxygen groups defined multiple times:"
    for group in ${MULTIPLE_DEFINED_GROUPS};
    do
        DEFGROUPS=$(echo "${ALL_RAW_DEFGROUP}" |
            awk -F@ '{ split($2, end, " "); printf("%s%s\n",$1,end[2]) }' |
            grep "\<${group}\>$" | sort -u)
        DEFGROUPFILES=$(echo "${DEFGROUPS}" | awk -F: '{ print "\t" $1 }')
        echo -e "\n${CWARN}${group}${CRESET} defined in:";
        echo "${DEFGROUPFILES}"
        echo "${DEFGROUPS}" | while read defgroup;
        do
            github_annotate_error $(echo ${defgroup} | awk -F: '{ print $1,$2 }') \
                "Multiple doxygen group define of '${group}' in\n${DEFGROUPFILES}"
        done
    done
    RESULT=2
fi
exit ${RESULT}
