BOARD ?= iotlab-m3
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

FEATURES_REQUIRED += periph_flashpage
FEATURES_OPTIONAL += periph_flashpage_raw
FEATURES_OPTIONAL += periph_flashpage_rwee

USEMODULE += shell

include $(RIOTBASE)/Makefile.include
