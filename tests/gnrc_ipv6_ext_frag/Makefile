DEVELHELP := 1
# name of your application
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := 
# chronos, hamilton, mips-malta, and ruuvitag boards don't support ethos
BOARD_BLACKLIST := chronos hamilton mips-malta ruuvitag

export TAP ?= tap0

CFLAGS += -DOUTPUT=TEXT
CFLAGS += -DTEST_SUITES="gnrc_ipv6_ext_frag"
CFLAGS += -DGNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE=3

# use Ethernet as link-layer protocol
ifeq (native,$(BOARD))
  USEMODULE += netdev_tap

  TERMFLAGS ?= $(TAP)
else
  USEMODULE += ethos

  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE) -DUSE_ETHOS_FOR_STDIO
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules for IPv6
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_error
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_pktbuf_cmd
# IPv6 extension headers
USEMODULE += gnrc_ipv6_ext_frag
# UDP support for payload
USEMODULE += gnrc_udp
USEMODULE += od
# Add unittest framework
USEMODULE += embunit
# Add also the shell, some shell commands
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# TEST_ON_CI_WHITELIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
