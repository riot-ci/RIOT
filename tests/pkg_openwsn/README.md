# OpenWSN on RIOT
This test demonstrates the [OpenWSN](https://github.com/openwsn-berkeley/openwsn-fw) full
stack (UDP, IPv6, RPL, 6TiSCH) running on RIOT. When flashed, it will initialize the stack
and provide the user with some minimal shell commands to:

- print the own IPv6 address `ifconfig`
- start a RPL root node `rplroot` (This is only available with `openwsn_serial`)
- change the UDP destination port `udp server start <port>`
- send a UDP packet `udp send <addr> <port> <data>`

Please note that this port is still in experimental status. For further information
about the port refer to the [pkg documentation](../../pkg/openwsn/doc.txt).

## Experimental setups

The following experiments act as a starting point for testing and debugging. Either
build and flash local nodes or incorporate the [FIT IoT-LAB](https://www.iot-lab.info/)
Testbed. Please check the ports [documentation](../../pkg/openwsn/doc.txt) for information
about supported hardware platforms.

This port currently needs `openvisualizer`, please make sure you follow the
[pre-requisites](dist/tools/openvisualizer/makefile.openvisualizer.inc.mk)
to install a patched version of `openvisualizer`.

If you want to disable channel hopping to speed up synchronization you can set
a fix channel:

    $ export CFLAGS=-DIEEE802154E_SINGLE_CHANNEL=17

## IMPORTANT!

OpenWSN uses source routing, this means all network traffic must go through
from the root node to OpenVisualizer. If the root node configuration can't
handle the configured baudrate correctly this will lead to packet loss.

Currently these are the tested configurations:

(a) samr21-xpro network:
    - lead nodes using `openwsn_sctimer_rtt`
    - root node using `openwsn_sctimer_rtt` and 19200 baudrate directly connect
      to UART pins (no through the usb debugger)

(b) iotlab-m3 network:
    - leaf nodes using `openwsn_sctimer_rtt` or `sctimer_ztimer`
    - root node using `openwsn_sctimer_rtt` and 57600 baudrate

For more on this please refer to [pkg documentation](../../pkg/openwsn/doc.txt).

### Launch an experiment

Assuming you have `iotlab-cli` installed, launch an experiment booking 3+
`iotlab-m3` nodes:

    $ iotlab-experiment submit -d 120 -l 3,archi=m3:at86rf231+site=saclay
    $ iotlab-experiment wait

    $ iotlab-experiment get --nodes

### Flash leaf node


Since multiple nodes where configured for the experiment `IOTLAB_NODE` needs
to be specified for every node

    $ IOTLAB_NODE=m3-1.saclay.iot-lab.info BOARD=iotlab-m3 make -C tests/pkg_openwsn flash
    $ IOTLAB_NODE=m3-2.saclay.iot-lab.info BOARD=iotlab-m3 make -C tests/pkg_openwsn flash

You can also start a terminal for these nodes

    $ IOTLAB_NODE=m3-2.saclay.iot-lab.info BOARD=iotlab-m3 make -C tests/pkg_openwsn term

    main(): This is RIOT! (Version: 2020.04-devel-1649-g96fa9-pr_openwsn)
    OpenWSN UDP test
    You are running RIOT on a(n) iotlab-m3 board.
    This board features a(n) stm32f1 MCU.
    > ifconfig
    ifconfig
    inet6 fe80::684:f665:106b:1114
    hwaddr short: 11:14    long: 06:84:F6:65:10:6B:11:14
    panid: CA:FE

    IEEE802154E sync: 0

    6tsch joined: 0

    NO RPL parent

### Flash root node

Same as above but with `openwsn_serial`:

    $ USEMODULE=openwsn_serial IOTLAB_NODE=m3-3.saclay.iot-lab.info BOARD=iotlab-m3 make -C tests/pkg_openwsn flash

The code that goes into root nodes and normal nodes is mostly the same except for
`stdio`. To be able to interface with `openvisualizer` the node will need a
dedicated uart. For most `BOARD`s its easier to simply disable `stdio` so this
application include `stdio_null` by default.

For the node you choose as root you can set it up with:

    $ IOTLAB_NODE=m3-3.saclay.iot-lab.info make -C tests/pkg_openwsn openv-termroot

This will launch openvisualizer and attach to the specified specified `PORT` or
in this case `IOTLAB_NODE`.

Your node will now start sending beacons and other nodes will synchronize, and
join. On leaf nodes yous should see the following:

    > [IEE20154E]: synchronized
    [neighbors]: new neighbor rssi: -66
    [cjoin]: send join request
    [cjoin]: success
    [icmpv6rpl]: found better parent
    [neighbors]: new neighbor rssi: -55
    ifconfig
    ifconfig
    inet6 bbbb::684:f665:106b:1114
    hwaddr short: 11:14    long: 06:84:F6:65:10:6B:11:14
    panid: CA:FE

    IEEE802154E sync: 1

    6tsch joined: 1

    RPL rank: 2816
    RPL parent: 2A:BA:F7:65:10:6B:11:14
    RPL DODAG ID: bbbb::2aba:f765:106b:1114

The root node should soon start receiving RPL DAOs:

    received RPL DAO from bbbb:0:0:0:ab8:fc65:106b:1114
    - parents:
    bbbb:0:0:0:2aba:f765:106b:1114
    - children:
    bbbb:0:0:0:684:f665:106b:1114

    received RPL DAO from bbbb:0:0:0:684:f665:106b:1114
    - parents:
    bbbb:0:0:0:2aba:f765:106b:1114
    - children:
    bbbb:0:0:0:ab8:fc65:106b:1114

    received RPL DAO from bbbb:0:0:0:684:f665:106b:1114
    - parents:
    bbbb:0:0:0:2aba:f765:106b:1114
    - children:
    bbbb:0:0:0:ab8:fc65:106b:1114

Once DAOs for all nodes start being received the network is setup and you
should be able to send packets between nodes or ping from the host.

### Send UDP packets

On one node setup a udp-server:

    ifconfig
    inet6 bbbb::684:f665:106b:1114
    hwaddr short: 11:14    long: 06:84:F6:65:10:6B:11:14
    panid: CA:FE

    IEEE802154E sync: 1

    6tsch joined: 1

    RPL rank: 2816
    RPL parent: 2A:BA:F7:65:10:6B:11:14
    RPL DODAG ID: bbbb::2aba:f765:106b:1114
    > udp server start 3000
    udp server start 3000
    Set UDP server port to 3000

On the other node send udp messages

    > udp send bbbb::684:f665:106b:1114 3000 hello
    udp send bbbb::684:f665:106b:1114 3000 hello
    Send 5 byte over UDP to [bbbb::684:f665:106b:1114]:3000
    > msg.l2_sendDoneError: 0
    Send success

The first node should receive the message

    > Received 12 bytes on port 3000
    00000000  A6  28  00  00  00  02  00  68  65  6C  6C  6F .(.....hello

### Communicating with host

OpenVisualizer can set up a tun interface to communicate with the host computer.
This will require starting `OpenVisualizer` with root privileges. The only
difference with the previous setup is that the root node must be setup as
follows:

    $ IOTLAB_NODE=m3-3.saclay.iot-lab.info make -C tests/pkg_openwsn openv-termtun

Once DAOs are received you can ping node in the network from your host:

```
$ ping6 -s 40 -i 5 bbbb:0:0:0:2ab5:fc65:106b:1114
PING bbbb:0:0:0:2ab5:fc65:106b:1114(bbbb::2ab5:fc65:106b:1114) 40 data bytes
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=1 ttl=64 time=1064 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=2 ttl=64 time=2111 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=3 ttl=64 time=1141 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=4 ttl=64 time=2197 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=5 ttl=64 time=1228 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=6 ttl=64 time=2306 ms
48 bytes from bbbb::2ab5:fc65:106b:1114: icmp_seq=7 ttl=64 time=1324 ms

```

Debug output if openserial is also used on leafnode:

```
16:02:38 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
16:02:44 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
16:02:48 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
16:02:54 [ParserIEC:INFO] 768f [ICMPv6ECHO] received an echo request
```

Some considerations:
    - Nodes duty cycle is ~0.5%, so nodes get a chance to transmit roughly every
      2s, so the worst case scenario is ~4s RTT. This is increased for big payloads
      since it will lead to fragmentation.
    - If incoming packet rate is too fast the internal packet queue can be
      be overloaded.
