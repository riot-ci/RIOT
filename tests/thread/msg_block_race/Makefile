DEVELHELP := 1

RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

DISABLE_MODULE += auto_init_xtimer
DISABLE_MODULE += auto_init_random

FEATURES_REQUIRED += periph_timer
USEMODULE += random

include $(RIOTBASE)/Makefile.include
