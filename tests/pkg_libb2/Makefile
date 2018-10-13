include ../Makefile.tests_common

# BLAKE2s + BLAKE2 is too big for these boards
BOARD_BLACKLIST := arduino-duemilanove arduino-uno mega-xplained waspmote-pro

TEST_ON_CI_WHITELIST += all

USEPKG += libb2
USEMODULE += embunit

include $(RIOTBASE)/Makefile.include
