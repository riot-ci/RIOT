#!/bin/bash
RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd "$(dirname "$0")" && pwd)
BLACKLIST=$CURDIR/blacklist.txt

. "$RIOTBASE"/dist/tools/ci/changed_files.sh
FILES=$(changed_files | grep -vf "$BLACKLIST")

check () {
    for F in $FILES
    do
        OUT="$(uncrustify -c "$CURDIR"/uncrustify-force.cfg -f "$RIOTBASE/$F" --check 2> /dev/null)"
        if [ "$OUT" ] ; then
            echo "Please run 'dist/tools/uncrustify/uncrustify.sh'"
            exit 1
        fi
    done
    echo "All files are uncrustified!"
}

exec_uncrustify () {
    if [ "$(git diff HEAD)" ] ; then
        echo "Please commit all changes before running uncrustify.sh"
        exit 1
    fi
    for F in $FILES
    do
        uncrustify -c "$CURDIR"/uncrustify-force.cfg --no-backup "$RIOTBASE/$F"
    done
}

if [ "$1" == "--check" ] ; then
    check
else
    exec_uncrustify
fi
