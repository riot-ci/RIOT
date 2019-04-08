include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := arduino-duemilanove

# msp430-libc does not provide `stdout` variable required by ubasic_tests
# newlib for esp8266 does not provide _times_r
BOARD_BLACKLIST := chronos esp8266-esp-12x esp8266-olimex-mod msb-430 msb-430h \
                   telosb wsn430-v1_3b wsn430-v1_4 z1

TEST_ON_CI_WHITELIST += all

USEPKG += ubasic
USEMODULE += ubasic_tests

include $(RIOTBASE)/Makefile.include
