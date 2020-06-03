BOARD ?= iotlab-m3

include ../Makefile.tests_common

USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler

# Optional Modules
#USEMODULE += openwsn_leds
#USEMODULE += openwsn_debugpins
#USEMODULE += openwsn_serial

# To enable debugging prints on OpenWSN code
# OPENWSN_LOG_LEVEL ?= LOG_DEBUG

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
  USEMODULE += stdio_null
endif

USEMODULE += ipv6_addr
USEMODULE += ps
USEMODULE += od_string
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include

# We want the highest possible frequency set for periph_rtt, but not all
# platforms can configure this value.
ifneq (,$(filter stm32f1,$(CPU)))
  CFLAGS += -DRTT_FREQUENCY=16384
endif