#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re
import os
import socket
import sys
import subprocess
import time

from scapy.all import IPv6, IPv6ExtHdrFragment, UDP
from testrunner import run


RECV_BUFSIZE = 2 * 1500
TEST_SAMPLE = b"This is a test. Failure might sometimes be an option, but " \
              b"not today. "
EXT_HDR_NH = {
    IPv6ExtHdrFragment: 44,
  }



def pktbuf_empty(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    first_byte = child.match.group("first_byte")
    size = child.match.group("size")
    child.expect(
            r"~ unused: {} \(next: (\(nil\)|0), size: {}\) ~".format(
                first_byte, size))


def pktbuf_size(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    size = child.match.group("size")
    return int(size)


def udp_send(child, addr, port, length, num=1, delay=1000000):
    child.sendline("udp send {addr} {port} {length} {num} {delay}"
                   .format(**vars()))
    child.expect("Success: send {length} byte to \[[0-9a-f:]+\]:{port}"
                 .format(**vars()))


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
            r"inet6 (?P<lladdr>[0-9A-Fa-f:]+)/64",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def get_host_mtu(tap):
    res = check_and_search_output(
            ["ip", "link", "show", tap],
            r"mtu (?P<mtu>1500)",
            "mtu"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return int(res)


def test_ipv6_ext_frag_shell_test_0(child, s, iface, ll_dst):
    child.sendline("test {} 0".format(ll_dst))
    data, _ = s.recvfrom(RECV_BUFSIZE)
    assert data == TEST_SAMPLE
    pktbuf_empty(child)


def test_ipv6_ext_frag_shell_test_1(child, s, iface, ll_dst):
    child.sendline("test {} 1".format(ll_dst))
    data, _ = s.recvfrom(RECV_BUFSIZE)
    offset = 0
    while (offset < len(data)):
        assert data[offset:(offset + len(TEST_SAMPLE))] == TEST_SAMPLE
        offset += len(TEST_SAMPLE)
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_success(child, s, iface, ll_dst):
    length = get_host_mtu(iface)
    port = s.getsockname()[1]
    udp_send(child, ll_dst, port, length)
    data, _ = s.recvfrom(length)
    assert len(data) == length
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_last_fragment_filled(child, s, iface, ll_dst):
    # every fragment has an IPv6 header and a fragmentation header so subtract
    # them
    mtu = get_host_mtu(iface) - len(IPv6() / IPv6ExtHdrFragment())
    # first fragment has UDP header (so subtract it) and is rounded down to
    # the nearest multiple of 8
    length = (mtu - len(UDP())) & 0xfff8
    # second fragment fills the whole available MTU
    length += mtu
    port = s.getsockname()[1]
    udp_send(child, ll_dst, port, length)
    data, _ = s.recvfrom(length)
    assert len(data) == length
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_last_fragment_only_one_byte(child, s,
                                                        iface, ll_dst):
    mtu = get_host_mtu(iface)
    # subtract IPv6 and UDP header as they are not part of the UDP payload
    length = (mtu - len(IPv6() / UDP()))
    length += 1
    port = s.getsockname()[1]
    udp_send(child, ll_dst, port, length)
    data, _ = s.recvfrom(length)
    assert len(data) == length
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_full_pktbuf(child, s, iface, ll_dst):
    length = pktbuf_size(child)
    # remove some slack for meta-data and header and 1 addition fragment header
    length -= (len(IPv6() / IPv6ExtHdrFragment() / UDP()) +
               (len(IPv6() / IPv6ExtHdrFragment())) + 96)
    port = s.getsockname()[1]
    # trigger neighbor discovery so it doesn't fill the packet buffer
    udp_send(child, ll_dst, port, 1)
    data, _ = s.recvfrom(1)
    last_nd = time.time()
    count = 0
    while True:
        if (time.time() - last_nd) > 5:
            # trigger neighbor discovery so it doesn't fill the packet buffer
            udp_send(child, ll_dst, port, 1)
            data, _ = s.recvfrom(1)
            last_nd = time.time()
        udp_send(child, ll_dst, port, length)
        count += 1
        try:
            data, _ = s.recvfrom(length)
        except socket.timeout:
            # 8 is the alignment unit of the packet buffer
            # and 20 the size of a packet snip, so take next multiple of 8 to
            # 28
            length -= 24
        else:
            break
        finally:
            pktbuf_empty(child)
    assert(count > 1)


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])

    def run_sock_test(func, s):
        if child.logfile == sys.stdout:
            func(child, s, tap, lladdr_src)
        else:
            try:
                func(child, s, tap, lladdr_src)
                print(".", end="", flush=True)
            except Exception as e:
                print("FAILED")
                raise e

    lladdr_src = get_host_lladdr(tap)

    child.expect(r"Sending UDP test packets to port (\d+)")
    port = int(child.match.group(1))
    with socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) as s:
        s.bind(("", port))
        s.setsockopt(socket.SOL_SOCKET, socket.SO_BINDTODEVICE,
                     str(tap + '\0').encode())
        s.settimeout(.3)
        run_sock_test(test_ipv6_ext_frag_shell_test_0, s)
        run_sock_test(test_ipv6_ext_frag_shell_test_1, s)
        run_sock_test(test_ipv6_ext_frag_send_success, s)
        run_sock_test(test_ipv6_ext_frag_send_last_fragment_filled, s)
        run_sock_test(test_ipv6_ext_frag_send_last_fragment_only_one_byte, s)
        run_sock_test(test_ipv6_ext_frag_send_full_pktbuf, s)
    print("SUCCESS")


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=2, echo=False))
