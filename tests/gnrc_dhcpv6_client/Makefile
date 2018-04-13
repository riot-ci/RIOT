include ../Makefile.tests_common

ifeq (,$(shell echo $$(command -v kea-dhcp6)))
  $(error "kea-dhcp6 required")
endif

# generate random free port
DHCPV6_SERVER_PORT := 61342

RIOTBASE ?= $(CURDIR)/../..

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo32-f042 \
                             nucleo32-f031 nucleo-f030 nucleo-l053 \
                             nucleo32-l031 stm32f0discovery telosb \
                             wsn430-v1_3b wsn430-v1_4 z1

# The following boards do not have an available UART
BOARD_BLACKLIST += mips-malta pic32-wifire pic32-clicker ruuvitag thingy52

USEMODULE += gnrc_dhcpv6_client
USEMODULE += gnrc_ipv6_default
USEMODULE += xtimer
ifneq (,$(filter native,$(BOARD)))
  USEMODULE += netdev_default
  IFACE ?= tapbr0
else
  USEMODULE += ethos
  IFACE ?= tap0
  ETHOS_BAUDRATE ?= 115200
  FEATURES_REQUIRED += periph_uart
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
  TERMPROG ?= sudo sh $(RIOTBASE)/dist/tools/ethos/ethos
  TERMFLAGS ?= $(IFACE) $(PORT) $(ETHOS_BAUDRATE)
  TERMDEPS += ethos
endif
USEMODULE += auto_init_gnrc_netif

USEMODULE += shell_commands

LOW_MEMORY_BOARDS := nucleo-f070 nucleo-f072 nucleo-f334 nucleo32-f303

ifeq ($(BOARD),$(filter $(BOARD),$(LOW_MEMORY_BOARDS)))
    CFLAGS += -DGNRC_PKTBUF_SIZE=512 -DGNRC_NETIF_IPV6_ADDRS_NUMOF=2 \
              -DGNRC_NETIF_IPV6_GROUPS_NUMOF=2 -DGNRC_IPV6_NIB_NUMOF=1 \
              -DNRC_IPV6_NIB_OFFL_NUMOF=1
endif

CFLAGS += -DDHCPV6_SERVER_PORT=$(DHCPV6_SERVER_PORT)

TERMDEPS += dhcpv6_server

include $(RIOTBASE)/Makefile.include


.PHONY: dhcpv6_server

dhcpv6_server:
	# sudo required since kea uses some PID and Lock files in /var
	sudo bash -x $(CURDIR)/dhcpv6_server.sh $(DHCPV6_SERVER_PORT) $(CURDIR)/kea-dhcp6.conf

ifeq (,$(filter native,$(BOARD)))
.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTBASE)/dist/tools/ethos
endif

test:
	# Require sudo for dhcpv6_server target and ethos
	sudo RIOTBASE="$(RIOTBASE)" ./tests/01-run.py
