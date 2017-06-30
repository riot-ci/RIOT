#!/bin/bash

ETHOS_DIR="$(cd "$(dirname "$0")" && pwd -P)"

configure_tap() {
    sysctl -w net.inet6.ip6.forwarding=1
    ifconfig tap0 inet6 fe80::1 prefixlen 64
    ifconfig lo0 inet6 fd00:dead:beef::1 prefixlen 128
    route -n add -inet6 -prefixlen 64 ${PREFIX} fe80::2
}

cleanup() {
    ifconfig lo0 inet6 -alias fd00:dead:beef::1
    route delete -inet6 ${PREFIX} -prefixlen 64 fe80::2
    sysctl -w net.inet6.ip6.forwarding=0
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ${UHCPD} ${TAP} ${PREFIX}
}

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200

UHCPD="$(cd "${ETHOS_DIR}/../uhcpd/bin" && pwd -P)/uhcpd"

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> [baudrate]"
    exit 1
}

[ ! -z $4 ] && {
    BAUDRATE=$4
}

trap "cleanup" INT QUIT TERM EXIT

configure_tap && start_uhcpd
