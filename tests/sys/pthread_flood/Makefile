RIOTBASE ?= $(CURDIR)/../../..
include $(RIOTBASE)/tests/Makefile.tests_common

USEMODULE += posix_headers
USEMODULE += pthread
CFLAGS += -DMAXTHREADS=8

include $(RIOTBASE)/Makefile.include
