#!/usr/bin/env python3

# Copyright (C) 2018   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import threading

from testrunner import run
from shared_func import TcpServer, generate_port_number, get_host_tap_device, \
                        get_host_ll_addr, get_riot_if_id, setup_internal_buffer, \
                        read_data_from_internal_buffer, verify_pktbuf_empty


def tcp_server(port, data):
    tcp_srv = TcpServer(port)
    tcp_srv.send(data)


def testfunc(child):
    port = generate_port_number()

    # Try to receive 2000 bytes sent from the Host System.
    data = '0123456789' * 200
    data_len = len(data)

    # Verify that RIOT Applications internal buffer can hold test data.
    assert setup_internal_buffer(child) >= data_len

    server_handle = threading.Thread(target=tcp_server, args=(port, data))
    server_handle.start()

    target_addr = get_host_ll_addr(get_host_tap_device()) + '%' + get_riot_if_id(child)

    # Setup RIOT Node to connect to Hostsystems TCP Server
    child.sendline('gnrc_tcp_tcb_init')
    child.sendline('gnrc_tcp_open_active AF_INET6 ' + target_addr + " " + str(port) + ' 0')
    child.expect_exact('gnrc_tcp_open_active: returns 0')

    # Accept Data sent by the host system
    child.sendline('gnrc_tcp_recv 0 ' + str(data_len))
    child.expect_exact('gnrc_tcp_recv: received ' + str(data_len))

    # Close connection and verify that pktbuf is cleared
    child.sendline('gnrc_tcp_close')
    server_handle.join()

    verify_pktbuf_empty(child)

    # Verify received Data
    assert read_data_from_internal_buffer(child, data_len) == data

    print(os.path.basename(sys.argv[0]) + ': success')


if __name__ == '__main__':
    sys.exit(run(testfunc, timeout=3, echo=False, traceback=True))
