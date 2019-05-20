## LoRaWAN

### Description

This application shows a basic LoRaWAN use-case with RIOT.

By using the real time clock and low-power capabilities of a board, this
application also shows how to program a LoRaWAN Class A device using RIOT.

By default, the application uses the Over-The-Air Activation (OTAA) procedure.

### Usage

To join a LoRaWAN network using OTAA activation, edit the application
`Makefile` and set your device information:

    ACTIVATION_MODE ?= otaa
    DEVEUI ?= 0000000000000000
    APPEUI ?= 0000000000000000
    APPKEY ?= 00000000000000000000000000000000

To join a LoRaWAN network using ABP activation, edit the application
`Makefile` and set your device and LoRaWAN application information:

    ACTIVATION_MODE ?= abp
    DEVADDR ?= 00000000
    NWKSKEY ?= 00000000000000000000000000000000
    APPSKEY ?= 00000000000000000000000000000000
    RX2_FREQ ?= 869525000
    RX2_DR ?= 3

Simply build and flash the application for a ST B-L072Z-LRWAN1 board:

    make flash term

Use the `BOARD`, `DRIVER` and `REGION` variables to adapt the application
to your hardware setup and region of use:

- `BOARD` can be one of the nucleo-64 boards
- `DRIVER` can be either `sx1276` or `sx1272`
- `REGION` can be `EU868`, `US915`, etc (see LoRaWAN regional parameters for
  details).

ST Nucleo-64 can be used as-is with mbed LoRa shields: there's one based on
[the sx1276 radio](https://os.mbed.com/components/SX1276MB1xAS/) and one based
on the [the sx1272 radio](https://os.mbed.com/components/SX1272MB2xAS/).
