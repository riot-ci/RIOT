APPLICATION = test_ssp
include ../Makefile.tests_common

# avr8 and msp430 don't support ssp
BOARD_BLACKLIST := arduino-mega2560 waspmote-pro arduino-uno arduino-duemilanove \
                   chronos msb-430 msb-430h telosb wsn430-v1_3b wsn430-v1_4 z1

USEMODULE += ssp

# set DEVELHELP so the board halts after crash
CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include
