# If no BOARD is found in the environment, use this default:
BOARD ?= samr21-xpro

RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

USEMODULE += isl29125
USEMODULE += xtimer

include $(RIOTBASE)/Makefile.include
