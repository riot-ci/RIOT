include ../Makefile.tests_common

# msp430 and avr have problems with int width and libcoaps usage of :x notation in structs
FEATURES_BLACKLIST += arch_avr8 arch_msp430

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo32-f031 nucleo32-f042 \
                             nucleo32-l031 nucleo-f030 nucleo-f070 nucleo-f334 nucleo-l053 \
                             stm32f0discovery telosb wsn430-v1_3b wsn430-v1_4 \
                             z1

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEPKG += libcoap

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
