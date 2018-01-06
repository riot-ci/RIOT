# About

This is a manual test application for testing the Arduino MKRWAN 1300 on-board
LoRa radio.

# Build and flash the application

1. First, put the board on bootloader mode by double pressing the reset button
2. Plug a serial to USB adapter on the Arduino RX/TX pin and open a terminal
on it (in general it's /dev/ttyUSB0 on Linux)
3. Issue the following command from RIOT base directory to flash the board:
```
    make BOARD=arduino-mkrwan1300 -C tests/driver_mkrwan flash
```

# Usage

This test application provides a shell with basic commands to interact with
the radio.

Note: the Device EUI of the LoRa cannot be changed.

* Read current version of the LoRa module:
```
> version
```

* Read the device EUI (invalid value given on purpose):
```
> get deveui
device eui: 0000000000000000
```

For an OTAA activation procedure:

* Set the application EUI (invalid value given on purpose):
```
> set appeui 0000000000000000
```

* Set the application key:
```
> set appkey 00000000000000000000000000000000
```

* Join the network:
```
> join otaa
Join procedure succeeded!
```

* Send some data to the LoRaWAN network:
```
> send ThisIsRIOT!
Data sent with success
```

You can also play with LoRaWAN basic parameters:

* Change the datarate to 5 (SF7BW125):
```
> set dr 5
```

* Turn off adaptive datarate:
```
> set adr off
```
