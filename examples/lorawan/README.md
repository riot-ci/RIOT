## LoRaWAN

### Description

This application shows a basic LoRaWAN use-case with RIOT.

By using the real time clock and low-power capabilities of a board, this
application also shows how to program a LoRaWAN Class A device using RIOT.

By default, the application uses the Over-The-Air Activation (OTAA) procedure.

### Configuration

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

Note that rx2 frequency (`RX2_FREQ`) and datarate (`RX2_DR`) variables must be
set explicitly at compile time when using ABP activation because they are
supposed to be known in advance by the network and the device. In this example,
the values used are compatible with TheThingsNetwork provider network.
They might change depending on the network provider used.

Use the `BOARD`, `DRIVER` and `LORA_REGION` variables to adapt the application
to your hardware setup and region of use:

- `BOARD` can be one of the nucleo-64 boards
- `DRIVER` can be either `sx1276` or `sx1272`
- `LORA_REGION` can be `EU868`, `US915`, etc (see LoRaWAN regional parameters for
  details).

The `SEND_PERIOD` variable can also be adapted to change the time period (in
seconds) between each message sent by the device.

### Usage

Simply build and flash the application for a ST B-L072Z-LRWAN1 board:

    make flash term

ST Nucleo-64 can be used as-is with mbed LoRa shields: there's one based on
[the sx1276 radio](https://os.mbed.com/components/SX1276MB1xAS/) and one based
on the [the sx1272 radio](https://os.mbed.com/components/SX1272MB2xAS/).
