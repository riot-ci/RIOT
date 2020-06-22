#!/bin/sh

export BOARD=samr21-xpro

set_node_env() {
    local I=0
    for node in $(edbg -l | grep -Pow 'ATML\d+' | sort); do
        echo "found node $I with serial $node"
        export NODE${I}=$node
        I=$(expr $I + 1)
    done
    if [ $I -lt 3 ]; then
        echo "less than three nodes found, aborting!"
        exit 1
    fi
}

flash_root_node() {
    echo ""
    echo "flashing root node..."
    set -e
    SERIAL=${NODE0} OPENSERIAL_BAUD=19200 USEMODULE=openwsn_serial \
        make flash -j4
}

flash_leaf_nodes() {
    echo ""
    echo "flashing leaf nodes..."
    set -e
    make all -j4
    make SERIAL=${NODE1} flash-only
    make SERIAL=${NODE2} flash-only
}

set_node_env
flash_root_node
flash_leaf_nodes

