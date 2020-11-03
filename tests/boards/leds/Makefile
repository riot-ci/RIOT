RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

# Some boards do not initialize LED0 by default
CFLAGS=-DAUTO_INIT_LED0

include $(RIOTBASE)/Makefile.include
