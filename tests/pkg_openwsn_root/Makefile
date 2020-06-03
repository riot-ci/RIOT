BOARD ?= iotlab-m3

include ../Makefile.tests_common

USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler
USEMODULE += openwsn_serial
USEMODULE += openwsn_leds
USEMODULE += openwsn_debugpins

USEMODULE += stdio_null

include $(RIOTBASE)/Makefile.include

# We want the highest possible frequency set for periph_rtt, but not all
# platforms can configure this value.
ifneq (,$(filter stm32f1,$(CPU)))
  CFLAGS += -DRTT_FREQUENCY=16384
endif