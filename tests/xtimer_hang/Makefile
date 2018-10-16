include ../Makefile.tests_common

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

USEMODULE += xtimer

TEST_ON_CI_WHITELIST += all

# Port and pin configuration for probing with oscilloscope
# Define Test pin for hardware timer interrupt, hardware dependent
# ATmega
CFLAGS += -DDEBUG_TIMER_PORT=PORTF
CFLAGS += -DDEBUG_TIMER_DDR=DDRF
CFLAGS += -DDEBUG_TIMER_PIN=PORTF4

# Define test probing pins GPIO API based.
# Port number should be found in port enum e.g in cpu/include/periph_cpu.h
FEATURES_REQUIRED += periph_gpio
# Jiminy probing Pins
CFLAGS += -DMAIN_THREAD_PIN=5
CFLAGS += -DMAIN_THREAD_PORT=PORT_F
CFLAGS += -DWORKER_THREAD_PIN=6
CFLAGS += -DWORKER_THREAD_PORT=PORT_F

include $(RIOTBASE)/Makefile.include
