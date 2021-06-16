DEVELHELP ?= 0
include ../Makefile.tests_common

# The test depends on how fast and often the host schedules the RIOT native
# application. If the host is busy enough it won't schedule the process,
# delaying the application, thus delaying the time measurement. This happens
# often on ci with resulting offsets of +10ms
TEST_ON_CI_BLACKLIST += native

USEMODULE += fmt
USEMODULE += ztimer_usec ztimer_msec

# ztimer_msec needs an RTT frequency of at least 1 kHz for a resolution of
# roughly 1 ms, but some boards set the RTT to 1 Hz by default. This should
# work around this at least for STM32, nRF52, and SAM*
ifneq (,$(filter stm32 nrf52 sam%,$(CPU)))
  RTT_FREQUENCY ?= RTT_MAX_FREQUENCY
  CFLAGS += -DRTT_FREQUENCY=$(RTT_FREQUENCY)
endif

include $(RIOTBASE)/Makefile.include
