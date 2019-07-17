#!/bin/sh

# This script resets a CC2538SF53 target using JLink called
# with a pre-defined reset sequence.

# @author Hauke Petersen <hauke.petersen@fu-berlin.de>

BINDIR=$1

# create JLink command file for resetting the board

echo "speed 1000" > $BINDIR/reset_active.seg
echo "r0" >> $BINDIR/reset_active.seg
echo "exit" >> $BINDIR/reset_active.seg
echo "speed 1000" > $BINDIR/reset_inactive.seg
echo "r1" >> $BINDIR/reset_inactive.seg
echo "exit" >> $BINDIR/reset_inactive.seg

JLinkExe -device CC2538SF53 < $BINDIR/reset_active.seg
sleep 0.1
JLinkExe -device CC2538SF53 < $BINDIR/reset_inactive.seg
echo ""
