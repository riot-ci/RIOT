include ../Makefile.tests_common

BOARD_PROVIDES_NETIF += airfy-beacon
BOARD_PROVIDES_NETIF += esp32-mh-et-live-minikit
BOARD_PROVIDES_NETIF += esp32-olimex-evb
BOARD_PROVIDES_NETIF += esp32-wemos-lolin-d32-pro
BOARD_PROVIDES_NETIF += esp32-wroom-32
BOARD_PROVIDES_NETIF += esp32-wrover-kit
BOARD_PROVIDES_NETIF += fox
BOARD_PROVIDES_NETIF += iotlab-m3
BOARD_PROVIDES_NETIF += msba2
BOARD_PROVIDES_NETIF += mulle
BOARD_PROVIDES_NETIF += native
BOARD_PROVIDES_NETIF += nrf51dk
BOARD_PROVIDES_NETIF += nrf51dongle
BOARD_PROVIDES_NETIF += nrf6310
BOARD_PROVIDES_NETIF += pba-d-01-kw2x
BOARD_PROVIDES_NETIF += samd21-xpro
BOARD_PROVIDES_NETIF += saml21-xpro
BOARD_PROVIDES_NETIF += samr21-xpro
BOARD_PROVIDES_NETIF += spark-core
BOARD_PROVIDES_NETIF += yunjia-nrf51822

BOARD_WHITELIST += $(BOARD_PROVIDES_NETIF)

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += gnrc
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
USEMODULE += netstats_l2

include $(RIOTBASE)/Makefile.include
