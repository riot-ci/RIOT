BOARD ?= iotlab-m3

include ../Makefile.tests_common

## OpenWSN Modules

USEPKG += openwsn
USEMODULE += openwsn_openstack
USEMODULE += openwsn_scheduler
# Optional Module required for root nodes
# USEMODULE += openwsn_serial
# Optional Modules
USEMODULE += openwsn_leds
USEMODULE += openwsn_debugpins

ifneq (,$(filter openwsn_serial,$(USEMODULE)))
# OpenWSN serial module can't handle data at more than 115200 bauds/s
OPENSERIAL_BAUD ?= 115200
CFLAGS += -DOPENWSN_UART_BAUDRATE=$(OPENSERIAL_BAUD)
# Uncomment to use STDIO_UART_DEV as the uart for OpenWSN openserial
# USEMODULE += stdio_null
  ifneq (,$(filter iotlab-m3 iotlab-a8-m3,$(BOARD)))
    USEMODULE += stdio_null
    BAUD ?= $(OPENSERIAL_BAUD)
  endif
endif

# To enable debugging prints on OpenWSN code
# OPENWSN_LOG_LEVEL ?= LOG_DEBUG

## Test application Modules
USEMODULE += ipv6_addr
USEMODULE += ps
USEMODULE += od_string
USEMODULE += shell
USEMODULE += shell_commands
# ztimer is used instead of xtimer because it's a dependency of some
# OpenWSN modules.
USEMODULE += ztimer_usec

# Additional networking modules that can be dropped if not needed
#USEMODULE += gnrc_icmpv6_echo

include $(RIOTBASE)/Makefile.include

# We want the highest possible frequency set for periph_rtt, but not all
# platforms can configure this value. use highest possible RTT_FREQUENCY
# for platforms that allow it
ifneq (,$(filter stm32,$(CPU)))
  RTT_FREQUENCY ?= RTT_MAX_FREQUENCY
  CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
endif

CFLAGS += -Wno-char-subscripts
