include ../Makefile.tests_common

USEMODULE += posix_headers
USEMODULE += pthread

# The test compiles, but these boards have too little RAM for
# dynamic allocation of thread stacks
BOARD_BLACKLIST := arduino-nano arduino-duemilanove \
                   arduino-uno nucleo-f031k6 stm32f030f4-demo

include $(RIOTBASE)/Makefile.include

# On msp430, the test needs a bit more than the default 256b
# stack. PTHREAD_STACKSIZE defaults to THREAD_STACKSIZE_DEFAULT.
ifneq (, $(filter msp430_common, $(USEMODULE)))
    CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=384
endif
