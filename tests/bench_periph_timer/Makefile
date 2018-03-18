include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer
USEMODULE += random
USEMODULE += fmt
USEMODULE += matstat

include $(RIOTBASE)/Makefile.include

# Shortcut to configure the build for testing Kinetis LPTMR against a PIT reference
# Usage: make BOARD=frdm-k22f test-kinetis-lptmr flash
test-kinetis-lptmr: CFLAGS+=-DTIM_TEST_DEV=TIMER_LPTMR_DEV\(0\) -DTIM_TEST_FREQ=32768 -DTIM_REF_DEV=TIMER_PIT_DEV\(0\)
test-kinetis-lptmr: all
