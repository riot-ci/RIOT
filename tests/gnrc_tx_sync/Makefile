include ../Makefile.tests_common

TEST_6LO ?= 1

ifeq (1,$(TEST_6LO))
  USEMODULE += gnrc_sixlowpan_default
  CFLAGS += -DCONFIG_GNRC_IPV6_NIB_SLAAC=1
else
  USEMODULE += gnrc_ipv6_default
endif

USEMODULE += gnrc_tx_sync
USEMODULE += netdev_test
USEMODULE += sock_udp
USEMODULE += xtimer
USEMODULE += auto_init_gnrc_netif
USEMODULE += iolist

include $(RIOTBASE)/Makefile.include

# This is needed for support of NETDEV_TYPE_TEST
CFLAGS += -DTEST_SUITES
