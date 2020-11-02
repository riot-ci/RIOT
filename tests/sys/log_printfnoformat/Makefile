RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

USEMODULE += log_printfnoformat

# Enable debug log level
CFLAGS += -DLOG_LEVEL=4

include $(RIOTBASE)/Makefile.include
