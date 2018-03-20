#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


import os
import scapy.all as scapy
import sys
import pexpect
import queue
import threading

link_layer_addr = None
l2_addr = None


class MockServer(threading.Thread):
    def __init__(self, exc_queue, ll_lock, timeout, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.exc_queue = exc_queue
        self.ll_lock = ll_lock
        self.timeout = timeout

    def run(self):
        try:
            DHCPV6_CLIENT_PORT = 546
            DHCPV6_SERVER_PORT = 547

            filter = "udp dst port %u" % DHCPV6_SERVER_PORT
            ps = scapy.sniff(iface=os.environ["IFACE"], filter=filter, count=1,
                             timeout=timeout)
            assert len(ps) == 1, "No DHCPv6 packet sent by client"
            p = ps[0]
            # wait for link-layer address
            if self.ll_lock.acquire(timeout=self.timeout):
                self.ll_lock.release()
            else:
                # pexpect failed => fall through to pexpect error
                return
            assert p["IPv6"].src == link_layer_addr
            assert p["UDP"].sport == DHCPV6_CLIENT_PORT
            try:
                # check DHCPv6 type and MUST options
                for check_type in ["DHCPv6 Solicit Message",
                                   "DHCP6 Client Identifier Option",
                                   # For RIOT other DUIDs do not make much
                                   # sense so it is safe to assume this DUID
                                   # will stick
                                   "DUID - Based on Link-layer Address",
                                   "DHCP6 Elapsed Time Option",
                                   "DHCP6 Option Request Option"
                                   ]:
                    p[check_type]
            except IndexError:
                assert False, "Received packet contained no %s" % check_type
            # check DUID correctness
            assert p["DUID - Based on Link-layer Address"].lladdr == l2_addr, \
                "DUID error: %s != %s" % (
                    p["DUID - Based on Link-layer Address"].lladdr,
                    l2_addr)
            # client is required to request SOL_MAX_RT option (82) from server
            assert 82 in p["DHCP6 Option Request Option"].reqopts
            # Try to crash client
            scapy.sendp(scapy.Ether(dst=p["Ethernet"].src) /
                        scapy.IPv6(dst=p["IPv6"].src) /
                        scapy.UDP(dport=p["UDP"].sport, sport=p["UDP"].dport) /
                        scapy.fuzz(
                                scapy.DHCP6(
                                        trid=p["DHCPv6 Solicit Message"].trid
                                    ) / scapy.Raw()
                            ),
                        iface=os.environ["IFACE"],
                        count=100)
            # TODO/XXX client doesn't listen for address leases yet, so no sense
            # in replying (since the client would interpret a ADVERTISE without
            # leases as invalid as per the RFC)

        except AssertionError as exc:
            self.exc_queue.put(exc)


def testfunc(child):
    global l2_addr, link_layer_addr
    child.expect(r"HWaddr:\s+([0-9a-f:]+)")
    l2_addr = child.match.group(1)
    child.expect(r"fe80::[0-9a-f:]+")
    link_layer_addr = child.match.group(0)
    ll_lock.release()
    try:
        child.expect("RIOT kernel panic")
    except pexpect.TIMEOUT:
        # we don't want a kernel panic ;-)
        pass
    else:
        assert False, "Node crashed"


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    timeout = 10
    # let mock server wait for link_layer_address
    ll_lock = threading.Lock()
    ll_lock.acquire()
    exc_queue = queue.Queue()
    t = MockServer(exc_queue, ll_lock, timeout)
    t.start()
    status = run(testfunc, timeout=5)
    t.join()
    try:
        exc = exc_queue.get(block=False)
    except queue.Empty:
        sys.exit(status)
    else:
        raise exc
