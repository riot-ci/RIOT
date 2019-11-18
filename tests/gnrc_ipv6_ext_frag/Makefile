DEVELHELP := 1
# name of your application
include ../Makefile.tests_common

export TAP ?= tap0

CFLAGS += -DOUTPUT=TEXT
CFLAGS += -DTEST_SUITES="gnrc_ipv6_ext_frag"
CFLAGS += -DGNRC_IPV6_EXT_FRAG_LIMITS_POOL_SIZE=3

ifeq (native,$(BOARD))
  TERMFLAGS ?= $(TAP)
else
  ETHOS_BAUDRATE ?= 115200
  CFLAGS += -DETHOS_BAUDRATE=$(ETHOS_BAUDRATE)
  TERMDEPS += ethos
  TERMPROG ?= sudo $(RIOTTOOLS)/ethos/ethos
  TERMFLAGS ?= $(TAP) $(PORT) $(ETHOS_BAUDRATE)
endif

# This application provides 2 network interfaces
GNRC_NETIF_NUMOF := 2

# The test requires some setup and to be run as root
# So it cannot currently be run
TEST_ON_CI_BLACKLIST += all

.PHONY: ethos

ethos:
	$(Q)env -u CC -u CFLAGS make -C $(RIOTTOOLS)/ethos

include $(RIOTBASE)/Makefile.include
