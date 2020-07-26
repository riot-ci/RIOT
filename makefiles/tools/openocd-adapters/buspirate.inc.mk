# Bus Pirate debug adapter
#
# For SWD mode connect:
#  MOSI - SWDIO
#   CLK - SWCLK

PROG_DEV ?= /dev/ttyUSB0
OPENOCD_ADAPTER_INIT ?= -c 'source [find interface/buspirate.cfg]' \
                        -c 'buspirate_port $(PROG_DEV)'
