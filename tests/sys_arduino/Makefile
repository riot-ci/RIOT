include ../Makefile.tests_common

USEMODULE += arduino

# Arduino module doesn't work on boards with stdio over USB
FEATURES_BLACKLIST += stdio_usb

include $(RIOTBASE)/Makefile.include
