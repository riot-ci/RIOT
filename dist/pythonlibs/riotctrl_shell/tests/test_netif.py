# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pytest
import riotctrl_shell.netif

from .common import init_ctrl


def test_netif_list_parser1():
    cmd_output = """
Iface  5  HWaddr: E2:BC:7D:CB:F5:50
          L2-PDU:1500  MTU:1500  HL:64  RTR
          RTR_ADV
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::e0bc:7dff:fecb:f550  scope: link  VAL
          inet6 addr: fe80::e0bc:7dff:fecb:f551  scope: link [anycast]  TNT[2]
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ffcb:f550

          White-listed link layer addresses:
             0: ab:cd:ef:01:23:45

          Statistics for Layer 2
            RX packets 14  bytes 1300
            TX packets 3 (Multicast: 1)  bytes 234
            TX succeeded 3 errors 0
          Statistics for IPv6
            RX packets 14  bytes 1104
            TX packets 3 (Multicast: 1)  bytes 192
            TX succeeded 3 errors 0"""
    parser = riotctrl_shell.netif.NetifListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 1
    assert "5" in res
    assert len(res["5"]["flags"]) == 2
    assert "RTR" in res["5"]["flags"]
    assert "RTR_ADV" in res["5"]["flags"]
    assert res["5"]["hwaddr"] == "E2:BC:7D:CB:F5:50"
    assert res["5"]["l2_pdu"] == 1500
    assert res["5"]["mtu"] == 1500
    assert res["5"]["hl"] == 64
    assert res["5"]["source_address_length"] == 6
    assert res["5"]["link_type"] == "wired"
    assert len(res["5"]["ipv6_addrs"]) == 2
    assert {"addr": "fe80::e0bc:7dff:fecb:f550", "scope": "link",
            "state": "VAL"} in res["5"]["ipv6_addrs"]
    assert {"addr": "fe80::e0bc:7dff:fecb:f551", "scope": "link",
            "state": "TNT[2]", "anycast": True} in res["5"]["ipv6_addrs"]
    assert len(res["5"]["ipv6_groups"]) == 3
    assert {"addr": "ff02::1"} in res["5"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["5"]["ipv6_groups"]
    assert {"addr": "ff02::1:ffcb:f550"} in res["5"]["ipv6_groups"]
    assert len(res["5"]["whitelist"]) == 1
    # white and blacklist are mutually exclusive
    assert "blacklist" not in res["5"]
    assert "ab:cd:ef:01:23:45" in res["5"]["whitelist"]
    assert len(res["5"]["stats"]) == 2
    assert res["5"]["stats"]["Layer 2"]["rx"]["packets"] == 14
    assert res["5"]["stats"]["Layer 2"]["rx"]["bytes"] == 1300
    assert res["5"]["stats"]["Layer 2"]["tx"]["packets"] == 3
    assert res["5"]["stats"]["Layer 2"]["tx"]["multicast"] == 1
    assert res["5"]["stats"]["Layer 2"]["tx"]["bytes"] == 234
    assert res["5"]["stats"]["Layer 2"]["tx"]["succeeded"] == 3
    assert res["5"]["stats"]["Layer 2"]["tx"]["errors"] == 0
    assert res["5"]["stats"]["IPv6"]["rx"]["packets"] == 14
    assert res["5"]["stats"]["IPv6"]["rx"]["bytes"] == 1104
    assert res["5"]["stats"]["IPv6"]["tx"]["packets"] == 3
    assert res["5"]["stats"]["IPv6"]["tx"]["multicast"] == 1
    assert res["5"]["stats"]["IPv6"]["tx"]["bytes"] == 192
    assert res["5"]["stats"]["IPv6"]["tx"]["succeeded"] == 3
    assert res["5"]["stats"]["IPv6"]["tx"]["errors"] == 0


def test_netif_list_parser2():
    cmd_output = """
Iface  7  HWaddr: 76:F5:98:9F:40:22
          L2-PDU:1500  MTU:1500  HL:64  RTR
          Source address length: 6
          Link type: wired
          inet6 addr: fe80::74f5:98ff:fe9f:4022  scope: link  VAL
          inet6 addr: fe80::2  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff9f:4022
          inet6 group: ff02::1:ff00:2

Iface  6  HWaddr: 2D:4A  Channel: 26  Page: 0  NID: 0x23  PHY: O-QPSK

          Long HWaddr: 5A:9D:93:86:22:08:65:7B
           TX-Power: 0dBm  State: IDLE  max. Retrans.: 3  CSMA Retries: 4
          AUTOACK  ACK_REQ  CSMA  L2-PDU:102  MTU:1280  HL:64  RTR
          RTR_ADV  6LO  IPHC
          Source address length: 8
          Link type: wireless
          inet6 addr: fe80::589d:9386:2208:657b  scope: link  VAL
          inet6 group: ff02::2
          inet6 group: ff02::1
          inet6 group: ff02::1:ff08:657b"""
    parser = riotctrl_shell.netif.NetifListParser()
    res = parser.parse(cmd_output)
    assert len(res) == 2
    assert "7" in res
    assert len(res["7"]["flags"]) == 1
    assert "RTR" in res["7"]["flags"]
    assert res["7"]["source_address_length"] == 6
    assert res["7"]["link_type"] == "wired"
    assert len(res["7"]["ipv6_addrs"]) == 2
    assert {"addr": "fe80::74f5:98ff:fe9f:4022", "scope": "link",
            "state": "VAL"} in res["7"]["ipv6_addrs"]
    assert {"addr": "fe80::2", "scope": "link",
            "state": "VAL"} in res["7"]["ipv6_addrs"]
    assert len(res["7"]["ipv6_groups"]) == 4
    assert {"addr": "ff02::1"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::1:ff9f:4022"} in res["7"]["ipv6_groups"]
    assert {"addr": "ff02::1:ff00:2"} in res["7"]["ipv6_groups"]
    assert "6" in res
    assert len(res["6"]["flags"]) == 7
    assert "AUTOACK" in res["6"]["flags"]
    assert "ACK_REQ" in res["6"]["flags"]
    assert "CSMA" in res["6"]["flags"]
    assert "RTR" in res["6"]["flags"]
    assert "RTR_ADV" in res["6"]["flags"]
    assert "6LO" in res["6"]["flags"]
    assert "IPHC" in res["6"]["flags"]
    assert res["6"]["hwaddr"] == "2D:4A"
    assert res["6"]["channel"] == 26
    assert res["6"]["page"] == 0
    assert res["6"]["nid"] == 0x23
    assert res["6"]["phy"] == "O-QPSK"
    assert res["6"]["long_hwaddr"] == "5A:9D:93:86:22:08:65:7B"
    assert res["6"]["tx_power"] == "0dBm"
    assert res["6"]["state"] == "IDLE"
    assert res["6"]["max_retrans"] == 3
    assert res["6"]["csma_retries"] == 4
    assert res["6"]["l2_pdu"] == 102
    assert res["6"]["mtu"] == 1280
    assert res["6"]["hl"] == 64
    assert res["6"]["source_address_length"] == 8
    assert res["6"]["link_type"] == "wireless"
    assert len(res["6"]["ipv6_addrs"]) == 1
    assert {"addr": "fe80::589d:9386:2208:657b", "scope": "link",
            "state": "VAL"} in res["6"]["ipv6_addrs"]
    assert {"addr": "ff02::1"} in res["6"]["ipv6_groups"]
    assert {"addr": "ff02::2"} in res["6"]["ipv6_groups"]
    assert {"addr": "ff02::1:ff08:657b"} in res["6"]["ipv6_groups"]


def test_netif_stats_parser():
    cmd_output = """
          Statistics for Layer 2
            RX packets 4  bytes 400
            TX packets 1 (Multicast: 1)  bytes 78
            TX succeeded 1 errors 0
          Statistics for IPv6
            RX packets 4  bytes 344
            TX packets 1 (Multicast: 1)  bytes 64
            TX succeeded 1 errors 0"""
    parser = riotctrl_shell.netif.NetifStatsParser()
    res = parser.parse(cmd_output)
    assert len(res) == 2
    assert res["Layer 2"]["rx"]["packets"] == 4
    assert res["Layer 2"]["rx"]["bytes"] == 400
    assert res["Layer 2"]["tx"]["packets"] == 1
    assert res["Layer 2"]["tx"]["multicast"] == 1
    assert res["Layer 2"]["tx"]["bytes"] == 78
    assert res["Layer 2"]["tx"]["succeeded"] == 1
    assert res["Layer 2"]["tx"]["errors"] == 0
    assert res["IPv6"]["rx"]["packets"] == 4
    assert res["IPv6"]["rx"]["bytes"] == 344
    assert res["IPv6"]["tx"]["packets"] == 1
    assert res["IPv6"]["tx"]["multicast"] == 1
    assert res["IPv6"]["tx"]["bytes"] == 64
    assert res["IPv6"]["tx"]["succeeded"] == 1
    assert res["IPv6"]["tx"]["errors"] == 0


def test_netif():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_list()
    # mock just returns last input
    assert res == "ifconfig"


def test_netif_w_netif():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_list("foobar")
    # mock just returns last input
    assert res == "ifconfig foobar"


def test_netif_cmd_empty():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_cmd()
    assert res == "ifconfig"


def test_netif_cmd_only_netif():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_cmd("foobar")
    assert res == "ifconfig foobar"


def test_netif_cmd_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(ValueError):
        si.netif_cmd(args=("test", "12345"))


def test_netif_help():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_help("foobar")
    assert res == "ifconfig foobar help"


def test_netif_set():
    rc = init_ctrl(output="success: address set")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_set("foobar", "addr", "42:de:ad:c0:ff:ee")
    assert res == "success: address set"
    assert rc.term.last_command == "ifconfig foobar set addr 42:de:ad:c0:ff:ee"


def test_netif_set_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_set("foobar", "addr", "42:de:ad:c0:ff:ee")
    assert rc.term.last_command == "ifconfig foobar set addr 42:de:ad:c0:ff:ee"


def test_netif_up():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    si.netif_up("foobar")
    assert rc.term.last_command == "ifconfig foobar up"


def test_netif_up_error():
    rc = init_ctrl("error: unable to set link foobar")
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_up("foobar")
    assert rc.term.last_command == "ifconfig foobar up"


def test_netif_down():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    si.netif_down("foobar")
    assert rc.term.last_command == "ifconfig foobar down"


def test_netif_down_error():
    rc = init_ctrl("error: unable to set link foobar")
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_down("foobar")
    assert rc.term.last_command == "ifconfig foobar down"


def test_netif_add():
    rc = init_ctrl(output="success: added address to interface")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_add("foobar", "dead:coff:ee::/64")
    assert res == "success: added address to interface"
    assert rc.term.last_command == "ifconfig foobar add dead:coff:ee::/64"


def test_netif_add_anycast():
    rc = init_ctrl(output="success: added address to interface")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_add("foobar", "dead:coff:ee::/64", anycast=True)
    assert res == "success: added address to interface"
    assert rc.term.last_command == \
        "ifconfig foobar add dead:coff:ee::/64 anycast"


def test_netif_add_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_add("foobar", "dead:coff:ee::/64")
    assert rc.term.last_command == "ifconfig foobar add dead:coff:ee::/64"


def test_netif_add_anycast_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_add("foobar", "dead:coff:ee::/64", anycast=True)
    assert rc.term.last_command == \
        "ifconfig foobar add dead:coff:ee::/64 anycast"


def test_netif_del():
    rc = init_ctrl(output="success: removed address from interface")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_del("foobar", "dead:coff:ee::/64")
    assert res == "success: removed address from interface"
    assert rc.term.last_command == "ifconfig foobar del dead:coff:ee::/64"


def test_netif_del_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_del("foobar", "dead:coff:ee::/64")
    assert rc.term.last_command == "ifconfig foobar del dead:coff:ee::/64"


def test_netif_flag_set():
    rc = init_ctrl(output="success: set option")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_flag("foobar", "6lo")
    assert res == "success: set option"
    assert rc.term.last_command == "ifconfig foobar 6lo"


def test_netif_flag_unset():
    rc = init_ctrl(output="success: set option")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_flag("foobar", "6lo", False)
    assert res == "success: set option"
    assert rc.term.last_command == "ifconfig foobar -6lo"


def test_netif_flag_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_flag("foobar", "6lo", False)
    assert rc.term.last_command == "ifconfig foobar -6lo"


def test_netif_l2filter_add():
    rc = init_ctrl(output="successfully added address to filter")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_l2filter_add("foobar", "ab:cd:ef:01:23:45")
    assert res == "successfully added address to filter"
    assert rc.term.last_command == \
        "ifconfig foobar l2filter add ab:cd:ef:01:23:45"


def test_netif_l2filter_add_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_l2filter_add("foobar", "ab:cd:ef:01:23:45")
    assert rc.term.last_command == \
        "ifconfig foobar l2filter add ab:cd:ef:01:23:45"


def test_netif_l2filter_del():
    rc = init_ctrl(output="successfully removed address to filter")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_l2filter_del("foobar", "ab:cd:ef:01:23:45")
    assert res == "successfully removed address to filter"
    assert rc.term.last_command == \
        "ifconfig foobar l2filter del ab:cd:ef:01:23:45"


def test_netif_l2filter_del_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_l2filter_del("foobar", "ab:cd:ef:01:23:45")
    assert rc.term.last_command == \
        "ifconfig foobar l2filter del ab:cd:ef:01:23:45"


def test_netif_stats():
    rc = init_ctrl(output="          Statistics for Layer 2\n          RX ...")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_stats("foobar", "l2")
    assert res.startswith("          Statistics for ")
    assert rc.term.last_command == \
        "ifconfig foobar stats l2"


def test_netif_stats_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_stats("foobar", "l2")
    assert rc.term.last_command == \
        "ifconfig foobar stats l2"


def test_netif_stats_reset():
    rc = init_ctrl(output="Reset statistics for module Layer 2!")
    si = riotctrl_shell.netif.Netif(rc)
    res = si.netif_stats_reset("foobar", "l2")
    assert res == "Reset statistics for module Layer 2!"
    assert rc.term.last_command == \
        "ifconfig foobar stats l2 reset"


def test_netif_stats_reset_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Netif(rc)
    with pytest.raises(RuntimeError):
        si.netif_stats_reset("foobar", "l2")
    assert rc.term.last_command == \
        "ifconfig foobar stats l2 reset"


def test_netif_txtsnd():
    rc = init_ctrl()
    si = riotctrl_shell.netif.NetifSend(rc)
    res = si.netif_txtsnd("foobar", "bcast", "abcdef")
    assert res == "txtsnd foobar bcast abcdef"


def test_netif_txtsnd_error1():
    rc = init_ctrl(output="error: foobar")
    si = riotctrl_shell.netif.NetifSend(rc)
    with pytest.raises(RuntimeError):
        si.netif_txtsnd("foobar", "bcast", "abcdef")
    assert rc.term.last_command == "txtsnd foobar bcast abcdef"


def test_netif_txtsnd_error2():
    rc = init_ctrl(output="usage: txtsnd foobar")
    si = riotctrl_shell.netif.NetifSend(rc)
    with pytest.raises(RuntimeError):
        si.netif_txtsnd("foobar", "bcast", "abcdef")
    assert rc.term.last_command == "txtsnd foobar bcast abcdef"
