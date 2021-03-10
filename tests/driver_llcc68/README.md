llcc68 LoRa driver
==================

This is a manual test application for the LLCC68 LoRa radio driver.

Usage
=====

This application adds a shell command to control basic features of the LLCC68
radio device:

```
main(): This is RIOT! (Version: 2021.04-devel)
Initialization successful - starting the shell now
> help
help
Command              Description
---------------------------------------
llcc68               Control the LLCC68 radio
reboot               Reboot the node
version              Prints current RIOT_VERSION
pm                   interact with layered PM subsystem
> llcc68
llcc68
Usage: llcc68 <get|set|rx|tx>
```

The `get` and `set` subcommands allows for getting/setting the current
frequency channel (freq) and lora modulation parameters (bw, sf, cr).

To put the device in listen mode, use the `rx` subcommand:

```
> llcc68 rx start
llcc68 rx start
Listen mode started
```

To send a message, use the `tx` subcommand:

```
> llcc68 tx "This is RIOT!"
llcc68 tx "This is RIOT!"
sending "This is RIOT!" payload (14 bytes)
> Transmission completed
```
