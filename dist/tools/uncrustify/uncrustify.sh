#!/bin/bash
RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd $(dirname $0) && pwd)
BLACKLIST=$CURDIR/blacklist.txt
FILES=$(git ls-tree -r HEAD --name-only --full-tree | grep -E '.*\.(c$|cpp$|h$|hpp$)' | grep -vf $BLACKLIST)

check () {
    for F in $FILES
    do
        OUT=$(diff $RIOTBASE/$F <(uncrustify -c $CURDIR/uncrustify-riot.cfg -f $RIOTBASE/$F 2>/dev/null))
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
        uncrustify -c $CURDIR/uncrustify-riot.cfg --no-backup $RIOTBASE/$F
    done
}

if [ "$1" == "--check" ] ; then
    check
else
    exec_uncrustify
fi
