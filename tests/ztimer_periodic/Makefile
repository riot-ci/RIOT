DEVELHELP ?= 0
include ../Makefile.tests_common

# The test depends on how fast and often the host schedules the RIOT native
# application. If the host is busy enough it won't schedule the process,
# delaying the application, thus delaying the time measurement. This happens
# often on ci with resulting offsets of +10ms
TEST_ON_CI_BLACKLIST += native

USEMODULE += fmt
USEMODULE += ztimer_usec ztimer_msec

# use periph_rtt backend for ZTIMER_MSEC for some boards to increase test coverage.
# note: Only add board if RTT_FREQUENCY >= 1 kHz
BOARDS_USE_RTT += nucleo-f767zi
BOARDS_USE_RTT += samr21-xpro

ifneq (,$(filter $(BOARD),$(BOARDS_USE_RTT)))
  USEMODULE += ztimer_periph_rtt
endif

include $(RIOTBASE)/Makefile.include
