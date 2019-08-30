include ../Makefile.tests_common

# Basic Configuration
BOARD ?= native
TAP ?= tap0

# Shorten default TCP timeouts to speedup testing
MSL_US ?= 1000000
TIMEOUT_US ?= 3000000

# Mark Boards with insufficient memory
BOARD_INSUFFICIENT_MEMORY :=

CFLAGS += -DSHELL_NO_ECHO
CFLAGS += -DGNRC_TCP_MSL=$(MSL_US)
CFLAGS += -DGNRC_TCP_CONNECTION_TIMEOUT_DURATION=$(TIMEOUT_US)

USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_tcp
USEMODULE += gnrc_pktbuf_cmd
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += od

ifeq (,$(filter native,$(BOARD)))
# Setup non-native (using ethos)
ETHOS_BAUDRATE ?= 115200

# Build RIOT with ethos support and use it with the test runner
CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
USEMODULE += stdio_ethos

TERMPROG ?= $(RIOTTOOLS)/ethos/ethos
TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
TERMDEPS += ethos

.PHONY:ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

else
# Setup native
PORT ?= $(TAP)
endif

# Export used tap device to environment
export TAPDEV = $(TAP)

include $(RIOTBASE)/Makefile.include
