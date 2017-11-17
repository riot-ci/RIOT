#!/bin/sh
#
# Copyright (c) 2017 Kaspar Schleiser <kaspar@schleiser.de>
#
# This script checks whether the board groups defined in boards/groups.mk
# contain and only contai boards that actually exist in the tree.

_greplist() {
    if [ $# -eq 0 ]; then
        echo cat
    else
        echo -n "grep -vE ($1"
        shift
        for i in $*; do
            echo -n "|$i"
        done
        echo ")"
    fi
}

: ${RIOTBASE:=$(pwd)}

export RIOTBASE

BOARD_LIST="$(make --no-print-directory -C${RIOTBASE}/dist/tools/boards list-boards)"
BOARD_FOLDERS="$(find ${RIOTBASE}/boards/* -maxdepth 0 -type d \! -name "*-common" -exec basename {} \;)"

if [ "$(echo ${BOARD_LIST} | sort)" != "$(echo ${BOARD_FOLDERS} | sort)" ]; then
    echo "$0: board list mismatch!"

    echo "list: $BOARD_LIST"
    echo "folders: $BOARD_FOLDERS"

    LIST_MISSING="$(echo "$BOARD_FOLDERS" | $(_greplist $BOARD_LIST))"
    FOLDER_MISSING="$(echo "$BOARD_LIST" | $(_greplist $BOARD_FOLDERS))"

    echo "list-missing: $LIST_MISSING"
    echo "folder-missing: $FOLDER_MISSING"

    [ -n "$LIST_MISSING" ] && {
        echo "Boards missing in boards/group.mk: $LIST_MISSING"
    }

    [ -n "$FOLDER_MISSING" ] && {
        echo "Boards listed in boards/group.mk without board folder: $FOLDER_MISSING"
    }

    exit 1
fi
true
