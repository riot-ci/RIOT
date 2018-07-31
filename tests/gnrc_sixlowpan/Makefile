# name of your application
include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := airfy-beacon chronos maple-mini msb-430 msb-430h \
                             nrf51dongle nrf6310 nucleo-f031k6 nucleo-f042k6 \
                             nucleo-l031k6 nucleo-f030r8 nucleo-f070rb nucleo-f103rb \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l053r8 \
                             spark-core stm32f0discovery telosb wsn430-v1_3b \
                             wsn430-v1_4 yunjia-nrf51822 z1

# use IEEE 802.15.4 as link-layer protocol
USEMODULE += netdev_ieee802154
USEMODULE += netdev_test
# 6LoWPAN and its extensions
USEMODULE += gnrc_sixlowpan_default
# UDP
USEMODULE += gnrc_udp
# Dumps packets
USEMODULE += gnrc_pktdump

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
