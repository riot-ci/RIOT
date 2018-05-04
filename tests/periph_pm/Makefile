BOARD ?= slstk3401a
include ../Makefile.tests_common

# method `fflush()` is not defined for MSP-430 (#6445 will fix this)
FEATURES_BLACKLIST += arch_msp430

FEATURES_OPTIONAL += periph_rtc

USEMODULE += shell

include $(RIOTBASE)/Makefile.include
