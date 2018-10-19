include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-mega2560 arduino-uno \
                   jiminy-mega256rfr2 mega-xplained waspmote-pro \
                   chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

TEST_ON_CI_WHITELIST += native

USEPKG += hacl
USEMODULE += random
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
