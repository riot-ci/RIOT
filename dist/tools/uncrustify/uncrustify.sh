#!/bin/bash
RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd $(dirname $0) && pwd)
BLACKLIST=$CURDIR/blacklist.txt
FILES=$(git ls-tree -r HEAD --name-only --full-tree | grep -E '.*\.(c$|cpp$|h$|hpp$)' | grep -vf $BLACKLIST)
for F in $FILES
do
	uncrustify -c $CURDIR/uncrustify-riot.cfg --no-backup $RIOTBASE/$F
done
