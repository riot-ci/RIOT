include ../Makefile.tests_common

# These boards only have a single timer in their periph_conf.h, needs special
# CFLAGS configuration to build properly
SINGLE_TIMER_BOARDS = nucleo32-f031 nucleo32-f042
SMALL_RAM_BOARDS = nucleo32-f031 nucleo32-f042

FEATURES_REQUIRED = periph_timer

USEMODULE += random
USEMODULE += fmt
USEMODULE += matstat

include $(RIOTBASE)/Makefile.include

ifeq (,$(findstring TIM_TEST_DEV,$(CFLAGS)))
  ifneq (,$(filter $(BOARD),$(SINGLE_TIMER_BOARDS)))
    CFLAGS += -DTIM_TEST_DEV=TIMER_DEV\(0\)
  endif
endif

ifeq (,$(findstring DETAILED_STATS,$(CFLAGS)))
  ifneq (,$(filter $(BOARD),$(SMALL_RAM_BOARDS)))
    CFLAGS += -DDETAILED_STATS=0
  endif
endif

# Shortcut to configure the build for testing Kinetis LPTMR against a PIT reference
# Usage: make BOARD=frdm-k22f test-kinetis-lptmr flash
test-kinetis-lptmr: CFLAGS+=-DTIM_TEST_DEV=TIMER_LPTMR_DEV\(0\) -DTIM_TEST_FREQ=32768 -DTIM_REF_DEV=TIMER_PIT_DEV\(0\)
test-kinetis-lptmr: all
