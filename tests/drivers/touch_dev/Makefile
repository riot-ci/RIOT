BOARD ?= stm32f429i-disc1
RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

DISABLE_MODULE += test_utils_interactive_sync

USEMODULE += stmpe811
USEMODULE += touch_dev
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
