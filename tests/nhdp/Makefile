include ../Makefile.tests_common

FEATURES_BLACKLIST += arch_avr8 arch_msp430

BOARD_INSUFFICIENT_MEMORY := nucleo32-f031 nucleo32-f042 nucleo32-l031 nucleo-f030 \
                             nucleo-f334 nucleo-l053 stm32f0discovery

USEMODULE += gnrc_ipv6
USEMODULE += gnrc_sock_udp
USEMODULE += nhdp

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include

test:
	tests/01-run.py
