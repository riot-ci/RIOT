OSNAME=`uname -s`
SEDBIN="sed -i"
if [ "${OSNAME}" = "Darwin" ] ; then
    SEDBIN="sed -i ''"
    LC_CTYPE=C
fi
