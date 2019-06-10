BOARD ?= samr21-xpro
include ../Makefile.tests_common

USEMODULE += auto_init_gnrc_netif
USEMODULE += auto_init_usbus
USEMODULE += gnrc_ipv6_router_default
USEMODULE += gnrc_icmpv6_echo
USEMODULE += usbus_cdc_ecm
USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Increase the number of network interfaces in case the board under test also provides a network interface
CFLAGS += -DGNRC_NETIF_NUMOF=2

# pid.codes test VID/PID, not globally unique
CFLAGS += -DUSB_CONFIG_VID=0x1209 -DUSB_CONFIG_PID=0x0001

include $(RIOTBASE)/Makefile.include
