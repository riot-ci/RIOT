#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import random
import re
import sys
import subprocess
import time
import threading

from scapy.all import Ether, IPv6, UDP, \
                      DNS, DNSQR, DNSRR, \
                      sendp, sniff
from testrunner import run


SERVER_TIMEOUT = 5
SERVER_ADDR = "2001:db8:0:d43f:93e5:5fff:fe3a:52ae"
SERVER_PORT = 53


class Server(threading.Thread):
    def __init__(self, iface, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.iface = iface
        self.stopped = False
        self.ps = []
        self.enter_loop = threading.Event()
        self.sniff_results = threading.Event()

    def run(self):
        while True:
            self.enter_loop.wait()
            self.enter_loop.clear()
            if self.stopped:
                return
            p = sniff(filter="udp and dst {} and port {:d}".format(
                    SERVER_ADDR, SERVER_PORT
                ), iface=self.iface, timeout=SERVER_TIMEOUT, count=1)[0]
            # check received packet for correctness
            assert(DNS in p)
            assert(p[DNS].qr == 0)
            assert(p[DNS].opcode == 0)
            # has two queries
            assert(p[DNS].qdcount == 2)
            # both for "example.org"
            assert(p[DNS].qd[0].qname == b"example.org.")
            assert(p[DNS].qd[1].qname == b"example.org.")
            assert(any(p[DNS].qd[i].qtype == 1 for i in range(2)))      # one is A
            assert(any(p[DNS].qd[i].qtype == 28 for i in range(2)))     # one is AAAA
            if self.reply is not None:
                self.reply[Ether].dst = p[Ether].src
                self.reply[Ether].src = p[Ether].dst
                self.reply[IPv6].dst = p[IPv6].src
                self.reply[IPv6].src = p[IPv6].dst
                self.reply[UDP].dport = p[UDP].sport
                self.reply[UDP].sport = p[UDP].dport
                sendp(self.reply, iface=self.iface, verbose=0)
                self.reply = None
            self.sniff_results.set()

    def listen(self, reply=None):
        self.reply = reply
        self.enter_loop.set()

    def stop(self):
        self.stopped = True
        self.enter_loop.set()
        self.join()


server = None


def check_and_search_output(cmd, pattern, res_group, *args, **kwargs):
    output = subprocess.check_output(cmd, *args, **kwargs).decode("utf-8")
    for line in output.splitlines():
        m = re.search(pattern, line)
        if m is not None:
            return m.group(res_group)
    return None


def get_bridge(tap):
    res = check_and_search_output(
            ["bridge", "link"],
            r"{}.+master\s+(?P<master>[^\s]+)".format(tap),
            "master"
        )
    return tap if res is None else res


def get_host_lladdr(tap):
    res = check_and_search_output(
            ["ip", "addr", "show", "dev", tap, "scope", "link"],
            r"inet6\s+(?P<lladdr>[0-9A-Fa-f:]+)/\d+",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def add_host_addr(tap, addr):
    subprocess.check_call(["ip", "address", "add", addr, "dev", tap])


def del_host_addr(tap, addr):
    subprocess.check_call(["ip", "address", "del", addr, "dev", tap])


def get_first_interface(child):
    child.sendline("ifconfig")
    child.expect(r"Iface\s+(\d+)")
    return int(child.match.group(1))


def add_default_route(child, iface, link):
    child.sendline("nib route add {} :: {}".format(iface, link))
    child.sendline("nib route")
    child.expect(r"default\* via {} dev #{}".format(link.lower(), iface))


def dns_server(child, server, port=53):
    child.sendline("dns server {} {:d}".format(server, port))
    child.sendline("dns server")
    child.expect(r"DNS server: \[{}\]:{:d}".format(server, port))


def successful_dns_request(child, name, exp_addr=None):
    child.sendline("dns request {}".format(name))
    res = child.expect(["error resolving {}".format(name),
                        "{} resolves to {}".format(name, exp_addr)],
                       timeout=3)
    return ((res > 0) and (exp_addr is not None))


def test_success(child):
    server.listen(Ether() / IPv6() / UDP() /
                  DNS(qr=1, qdcount=2, ancount=2,
                      qd=(DNSQR(qname="example.org", qtype=28) /
                          DNSQR(qname="example.org", qtype=1)),
                      an=(DNSRR(rrname="example.org", type=28,
                                rdlen=16, rdata="2001:db8::1") /
                          DNSRR(rrname="example.org", type=1,
                                rdlen=4, rdata="10.0.0.1"))))
    assert(successful_dns_request(child, "example.org", "2001:db8::1"))


def testfunc(child):
    global server
    tap = get_bridge(os.environ["TAP"])
    lladdr = get_host_lladdr(tap)
    add_host_addr(tap, SERVER_ADDR)

    try:
        iface = get_first_interface(child)
        add_default_route(child, iface, lladdr)
        dns_server(child, SERVER_ADDR)

        server = Server(tap)
        server.start()

        def run(func):
            if child.logfile == sys.stdout:
                print(func.__name__)
                func(child)
            else:
                try:
                    func(child)
                    print(".", end="", flush=True)
                except Exception as e:
                    print("FAILED")
                    raise e

        run(test_success)
        print("SUCCESS")
    finally:
        del_host_addr(tap, SERVER_ADDR)
        if server is not None:
            server.stop()


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=1, echo=False))
