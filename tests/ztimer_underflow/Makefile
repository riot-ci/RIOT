include ../Makefile.tests_common

USEMODULE += ztimer

# Which clock shall be tested?
# -> ZTIMER_USEC
USEMODULE += ztimer_usec
CFLAGS += -DTEST_ZTIMER_CLOCK=ZTIMER_USEC
# -> ZTIMER_MSEC
#USEMODULE += ztimer_msec ztimer_periph_rtt
#CFLAGS += -DTEST_ZTIMER_CLOCK=ZTIMER_MSEC

include $(RIOTBASE)/Makefile.include
