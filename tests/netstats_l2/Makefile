include ../Makefile.tests_common

# This application runs by default on boards providing the netif feature
FEATURES_REQUIRED += netif

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2

include $(RIOTBASE)/Makefile.include
