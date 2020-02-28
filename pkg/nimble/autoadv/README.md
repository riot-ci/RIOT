# NimBLE automated advertisements

Module for automated bluetooth advertising. Advertising is restarted on
disconnect events automatically. Defaults to the following characteristics:
    - General discoverable mode (BLE_GAP_DISC_MODE_GEN)
    - Undirected connectable mode (BLE_GAP_CONN_MODE_UND)
    - No expiration (BLE_HS_FOREVER)
    - No name

It can be enabled by adding
```
USEMODULE += nimble_autoadv
```
to your makefile.

To specify a device name add the following line to your Makefile:
```
CFLAGS += -DNIMBLE_AUTOADV_DEVICE_NAME='"Riot OS device"'
```
