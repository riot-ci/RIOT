include ../Makefile.tests_common

FEATURES_REQUIRED = periph_timer_periodic

# make sure the test still fits on small RAM boards
CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512

include $(RIOTBASE)/Makefile.include
