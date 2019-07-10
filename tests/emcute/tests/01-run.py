#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import pprint
import random
import re
import socket
import sys
import subprocess
import time

from scapy.all import Automaton, ATMT, log_runtime, MTU, raw, SimpleSocket
import scapy_mqttsn as mqttsn
from testrunner import run

SERVER_PORT = 1883
MODES = set(["pub", "sub", "sub_w_reg"])
TIMEOUT = 1


class MQTTSNServer(Automaton):
    class MQTTSNServerSocket(SimpleSocket):
        def __init__(self, server, *args, **kwargs):
            super(MQTTSNServer.MQTTSNServerSocket, self)\
                .__init__(*args, **kwargs)
            self.server = server

        def recv(self, x=MTU):
            pkt, sa = self.ins.recvfrom(x)
            self.server.last_remote = sa
            return mqttsn.MQTTSN(pkt)

        def send(self, x):
            assert self.server.last_remote is not None
            try:
                sx = raw(x)
                x.sent_time = time.time()
                self.outs.sendto(sx, self.server.last_remote)
            except socket.error as msg:
                log_runtime.error(msg)

    def __init__(self, *args, **kwargs):
        kwargs["ll"] = MQTTSNServer.MQTTSNServerSocket
        kwargs["recvsock"] = MQTTSNServer.MQTTSNServerSocket
        self.last_remote = None
        super(MQTTSNServer, self).__init__(*args, **kwargs)

    def parse_args(self, spawn, bind_addr, topic_name, mode, pub_interval,
                   qos_level=0,
                   data_len_start=1, data_len_end=1000, data_len_step=1,
                   bind_port=SERVER_PORT, family=socket.AF_INET,
                   type=socket.SOCK_DGRAM, proto=0, *args, **kwargs):
        assert mode in MODES
        super(MQTTSNServer, self).parse_args(*args, **kwargs)
        self.spawn = spawn
        self.topic_name = topic_name
        self.mode = mode
        self.pub_interval = pub_interval
        self.qos_level = qos_level
        self.data_len = data_len_start
        self.data_len_end = data_len_end
        self.data_len_step = data_len_step
        self.last_mid = random.randint(0, 0xffff)
        self.topics = []
        self.registered_topics = []
        self.subscriptions = []
        self.res = ""

        sock = socket.socket(family, type, proto)
        res = socket.getaddrinfo(bind_addr, bind_port)
        sockaddr = res[0][4]
        sock.bind(sockaddr)
        self.gw_addr = "[{}]:{}".format(sockaddr[0], sockaddr[1])
        self.socket_kargs = {"sock": sock, "server": self}

    def _get_tid(self, topic_name):
        if topic_name not in self.topics:
            self.topics.append(topic_name)
        return self.topics.index(topic_name) + 1

    def _get_topic_name(self, tid):
        return self.topics[tid - 1]

    @property
    def _qos_flags(self):
        qos = min(self.qos_level, 2)
        if qos < 0:
            qos = mqttsn.QOS_NEG1
        return qos

    @ATMT.state(initial=1)
    def BEGIN(self):
        self.spawn.expect_exact("success: starting test application")
        raise self.CONNECTING()

    @ATMT.state()
    def CONNECTING(self):
        self.spawn.sendline("con {}".format(self.gw_addr))
        raise self.WAITING_CONNECT()

    @ATMT.state()
    def WAITING_CONNECT(self):
        pass

    @ATMT.timeout(WAITING_CONNECT, TIMEOUT)
    def timeout_CONNECT(self):
        raise self.MESSAGE_TIMEOUT("CONNECT")

    @ATMT.receive_condition(WAITING_CONNECT)
    def receive_CONNECT(self, pkt):
        if pkt.type == mqttsn.CONNECT:
            if self.mode in ["pub", "sub_w_reg"]:
                raise self.REGISTERING()
            elif self.mode in ["sub"]:
                raise self.SUBSCRIBING()
        else:
            raise self.UNEXPECTED_MESSAGE_TYPE(pkt.type)

    @ATMT.action(receive_CONNECT)
    def send_CONACK(self):
        self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNConnack()
        self.send(self.last_packet)
        self.spawn.expect_exact("success: connected to gateway at {}"
                                .format(self.gw_addr))

    @ATMT.state()
    def REGISTERING(self):
        self.spawn.sendline("reg {}".format(self.topic_name))
        raise self.WAITING_REGISTER()

    @ATMT.state()
    def WAITING_REGISTER(self):
        pass

    @ATMT.timeout(WAITING_REGISTER, TIMEOUT)
    def timeout_REGISTER(self):
        raise self.MESSAGE_TIMEOUT("REGISTER")

    @ATMT.receive_condition(WAITING_REGISTER)
    def receive_REGISTER(self, pkt):
        if pkt.type == mqttsn.REGISTER:
            topic_name = pkt.topic_name.decode()
            if self.mode in ["pub"]:
                raise self.PUBLISHING(topic_name).action_parameters(topic_name,
                                                                    pkt.mid)
            else:
                raise self.SUBSCRIBING().action_parameters(topic_name, pkt.mid)
        else:
            raise self.UNEXPECTED_MESSAGE_TYPE(pkt.type)

    @ATMT.action(receive_REGISTER)
    def send_REGACK(self, topic_name, mid):
        tid = self._get_tid(topic_name)
        if topic_name not in self.registered_topics:
            self.registered_topics.append(topic_name)
        self.last_packet = mqttsn.MQTTSN() / \
            mqttsn.MQTTSNRegack(mid=mid, tid=tid)
        self.send(self.last_packet)
        self.spawn.expect_exact("success: registered to topic '{} [{:d}]'"
                                .format(topic_name, tid))

    @ATMT.state()
    def PUBLISHING(self, topic_name):
        if self.data_len < self.data_len_end:
            self.spawn.sendline("pub {} {:d} {:d}" .format(topic_name,
                                                           self.data_len,
                                                           self.qos_level))
            raise self.WAITING_PUBLISH()
        else:
            raise self.END()

    @ATMT.state()
    def WAITING_PUBLISH(self):
        pass

    @ATMT.timeout(WAITING_PUBLISH, TIMEOUT)
    def timeout_PUBLISH(self):
        raise self.MESSAGE_TIMEOUT("PUBLISH")

    @ATMT.receive_condition(WAITING_PUBLISH)
    def receive_PUBLISH(self, pkt):
        if pkt.type == mqttsn.PUBLISH and \
            (((pkt.qos == mqttsn.QOS_NEG1) and (self.qos_level == -1)) or
             (pkt.qos == self.qos_level)):
            assert self.data_len == len(pkt.data)
            topic_name = self._get_topic_name(pkt.tid)
            self.res += ":".join("{:02x}".format(c) for c in pkt.data)
            self.data_len += self.data_len_step
            raise self.PUBLISHING(topic_name).action_parameters(pkt.qos,
                                                                pkt.mid,
                                                                topic_name,
                                                                pkt.tid)
        else:
            raise self.UNEXPECTED_MESSAGE(pkt.type, pkt.qos)

    @ATMT.action(receive_PUBLISH)
    def send_PUBACK(self, qos_level, mid, topic_name, tid):
        if qos_level in (mqttsn.QOS_1, mqttsn.QOS_2):
            self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNPuback(mid=mid,
                                                                     tid=tid)
            self.send(self.last_packet)
        self.spawn.expect_exact(
            "success: published {:d} bytes to topic '{} [{:d}]'"
            .format(self.data_len - self.data_len_step, topic_name, tid)
        )
        time.sleep(self.pub_interval)

    @ATMT.state()
    def SUBSCRIBING(self):
        self.spawn.sendline("sub {} {}".format(self.topic_name,
                                               self.qos_level))
        raise self.WAITING_SUBSCRIBE()

    @ATMT.state()
    def WAITING_SUBSCRIBE(self):
        pass

    @ATMT.timeout(WAITING_SUBSCRIBE, TIMEOUT)
    def timeout_SUBSCRIBE(self):
        raise self.MESSAGE_TIMEOUT("SUBSCRIBE")

    @ATMT.receive_condition(WAITING_SUBSCRIBE)
    def receive_SUBSCRIBE(self, pkt):
        if pkt.type == mqttsn.SUBSCRIBE and \
            (((pkt.qos == mqttsn.QOS_NEG1) and (self.qos_level == -1)) or
             (pkt.qos == self.qos_level)):
            if pkt.tid_type in [mqttsn.TID_NORMAL, mqttsn.TID_SHORT]:
                topic_name = pkt.topic_name
                tid = self._get_tid(pkt.topic_name)
            elif pkt.tid_type == mqttsn.TID_PREDEF:
                tid = pkt.tid
                topic_name = self._get_topic_name(tid)
            else:
                assert(False)
            subscription = {"tid": tid, "topic_name": topic_name}
            if subscription not in self.subscriptions:
                self.subscriptions.append(subscription)
            raise self.PUBLISH_TO_SUBSCRIBER(subscription).action_parameters(
                tid=tid, mid=pkt.mid
            )
        else:
            raise self.UNEXPECTED_MESSAGE(pkt.type, pkt.qos)

    @ATMT.action(receive_SUBSCRIBE)
    def send_SUBACK(self, tid, mid):
        self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNSuback(
            tid=tid, mid=mid
        )
        self.send(self.last_packet)
        self.spawn.expect_exact("success: now subscribed to {}"
                                .format(self._get_topic_name(tid).decode()))

    def _publish_to_subscriber(self, subscription):
        if self.data_len > 0:
            self.spawn.expect_exact(
                    "### got publication of {:d} bytes for topic "
                    "'{}' [{:d}] ###"
                    .format(self.data_len, subscription["topic_name"].decode(),
                            subscription["tid"]))
        self.data_len += self.data_len_step
        time.sleep(self.pub_interval)
        raise self.PUBLISH_TO_SUBSCRIBER(subscription)

    @ATMT.state()
    def PUBLISH_TO_SUBSCRIBER(self, subscription):
        tid = subscription["tid"]
        self.last_mid += 1
        mid = self.last_mid
        if self.data_len == 0:
            # send deliberately broken length packets
            # (to small payload, len field < 256)
            self.last_packet = mqttsn.MQTTSN(len=128) / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="128"
            )
            self.send(self.last_packet)
            # send deliberately broken length packets
            # (to small payload, len field >= 256)
            self.last_packet = mqttsn.MQTTSN(len=400) / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="400"
            )
            self.send(self.last_packet)
            # send deliberately broken length packets (too large payload)
            self.last_packet = mqttsn.MQTTSN(len=10) / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="X" * 20
            )
            self.send(self.last_packet)
            # last message should be received truncated
            if self.last_packet.qos in [mqttsn.QOS_1, mqttsn.QOS_2]:
                raise self.WAITING_PUBACK(tid, mid)
            else:
                self._publish_to_subscriber(subscription)
        if self.data_len < self.data_len_end:
            self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="X" * self.data_len
            )
            self.send(self.last_packet)
            if self.last_packet.qos in [mqttsn.QOS_1, mqttsn.QOS_2]:
                raise self.WAITING_PUBACK(tid, mid)
            else:
                self._publish_to_subscriber(subscription)
        else:
            raise self.END()

    @ATMT.state()
    def WAITING_PUBACK(self, tid, mid):
        self.expected_pub_tid = tid
        self.expected_pub_mid = mid

    @ATMT.timeout(WAITING_PUBACK, TIMEOUT)
    def timeout_PUBACK(self):
        raise self.MESSAGE_TIMEOUT("PUBACK")

    @ATMT.receive_condition(WAITING_PUBACK)
    def receive_PUBACK(self, pkt):
        if pkt.type == mqttsn.PUBACK:
            assert self.expected_pub_tid == pkt.tid
            assert self.expected_pub_mid == pkt.mid
            assert mqttsn.ACCEPTED == pkt.return_code
            self._publish_to_subscriber(
                {"tid": pkt.tid, "topic_name": self._get_topic_name(pkt.tid)}
            )
        else:
            raise self.UNEXPECTED_MESSAGE_TYPE(pkt.type)

    @ATMT.state(final=1)
    def END(self):
        self.spawn.sendline("info")
        self.spawn.expect_exact("Broker: '{}'".format(self.gw_addr))
        self.spawn.expect_exact("- Topics")
        for tid, topic_name in enumerate(self.registered_topics, 1):
            self.spawn.expect_exact("  {:2d}: {}".format(tid, topic_name))
        self.spawn.expect_exact("- Subscriptions")
        for sub in self.subscriptions:
            self.spawn.expect_exact("  {:2d}: {}".format(
                sub["tid"], sub["topic_name"].decode())
            )
        self.spawn.sendline("reboot")
        return self.res

    @ATMT.state(error=1)
    def UNEXPECTED_MESSAGE_TYPE(self, type, qos=None):
        self.res += "\nUnexpected message type {} {}".format(type, qos)
        return self.res

    @ATMT.state(error=1)
    def MESSAGE_TIMEOUT(self, state):
        self.res += "\n{} timed out".format(state)
        return self.res


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
                "Can't find host link-local address on interface {}"
                .format(tap)
            )
    else:
        return res


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])
    lladdr = get_host_lladdr(tap)

    DATA_MAX_LEN = 512 - 9  # PUBLISH + 2 byte extra for length
    TOPIC_MAX_LEN = 249     # see Makefile
    for test_params in [
        {"qos_level": 0, "mode": "sub", "topic_name": "/test",
         "data_len_start": 0, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50},
        {"qos_level": 1, "mode": "sub", "topic_name": "/test",
         "data_len_start": 0, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50},
        {"qos_level": 1, "mode": "sub",
         "topic_name": "/" + ("x" * (TOPIC_MAX_LEN - 1)),
         "data_len_start": 8, "data_len_end": 9},
        {"qos_level": 1, "mode": "sub_w_reg", "topic_name": "/test",
         "data_len_start": 8, "data_len_end": 9},
        {"qos_level": 0, "mode": "pub", "topic_name": "/test",
         "data_len_start": 1, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50},
        {"qos_level": 1, "mode": "pub", "topic_name": "/test",
         "data_len_start": 1, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50}
    ]:
        print("Run test case")
        pprint.pprint(test_params, compact=False)
        server = MQTTSNServer(child, pub_interval=.001,
                              family=socket.AF_INET6,
                              bind_addr=lladdr + "%" + tap,
                              bind_port=SERVER_PORT, **test_params)
        try:
            server.run()
        finally:
            server.stop()
            server.socket_kargs["sock"].close()
            time.sleep(1)
    print("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT, echo=False))
