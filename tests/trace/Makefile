include ../Makefile.tests_common

USEMODULE += trace

include $(RIOTBASE)/Makefile.include

# reduce tracebuffer (default is 512), so this test compiles for more boards
ifndef CONFIG_TRACE_BUFSIZE
  CFLAGS += -DCONFIG_TRACE_BUFSIZE=64
endif
