# Overview

This folder contains a test application for RIOT's DHCPv6 client.

# How to test

The test script requires the `scapy-python3` python package:

```sh
pip3 install scapy-python3
```

If you use any platform other than `native`, you need to use `ethos`, otherwise
`netdev_tap` is chosen.

Use the `IFACE` environment variable to choose the tap interface (or bridge) is
connected to. By default `tapbr0` is assumed for `native` (as created by the
`dist/tools/tapsetup/tapsetup` script), and `tap0` for other platforms. See an
example below:

```sh
BOARD=samr21-xpro IFACE=tap0 make all test
```

You don't need to start any DHCPv6 server.
