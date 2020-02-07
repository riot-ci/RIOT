DEVELHELP := 1
include $(CURDIR)/../Makefile.tests_common

export TAP ?= tap0
GNRC_NETIF_NUMOF := 2

USEMODULE += auto_init_gnrc_netif
USEMODULE += gnrc_dhcpv6_client_6lbr
USEMODULE += gnrc_netdev_default
USEMODULE += gnrc_pktdump
USEMODULE += gnrc_sixlowpan_border_router_default
USEMODULE += ps
USEMODULE += shell
USEMODULE += shell_commands

# use Ethernet as link-layer protocol
ifeq (native,$(BOARD))
  TERMFLAGS += -z [::1]:17754
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

# The test requires some setup and to be run as root
# So it cannot currently be run on CI
TEST_ON_CI_BLACKLIST += all

include $(RIOTBASE)/Makefile.include

ifeq (,$(filter native,$(BOARD)))
.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTBASE)/dist/tools/ethos
endif
