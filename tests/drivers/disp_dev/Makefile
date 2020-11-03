BOARD ?= stm32f429i-disc1
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += ili9341
USEMODULE += disp_dev

include $(RIOTBASE)/Makefile.include
