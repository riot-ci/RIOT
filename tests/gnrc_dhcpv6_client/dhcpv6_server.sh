#! /bin/bash
#
# random_port.sh
# Copyright (C) 2018 Martine Lenders <m.lenders@fu-berlin.de>
#
# Distributed under terms of the MIT license.
#

_dhcpv6_server() {
    sleep 1 # sleep to let TAP become active
    kea-dhcp6 -p $1 -c $2
}

# no need to kill from external, kea handles double instances gracefully
_dhcpv6_server $1 $2 &
