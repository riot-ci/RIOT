#!/bin/bash
RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd $(dirname $0) && pwd)
BLACKLIST=$CURDIR/blacklist.txt
FILES=$(git ls-tree -r HEAD --name-only --full-tree | grep -E '.*\.(c|cpp|h|hpp)' | grep -vf $BLACKLIST)
for F in $FILES
do
	OUT=$(diff $RIOTBASE/$F <(uncrustify -c $CURDIR/uncrustify-riot.cfg -f $RIOTBASE/$F 2>/dev/null))
	if [ "$OUT" ] ; then
		echo "Please run 'dist/tools/uncrustify/uncrustify.sh'"
		exit 1
	fi
done
