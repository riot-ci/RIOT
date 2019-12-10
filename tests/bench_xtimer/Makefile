include ../Makefile.tests_common

USEMODULE += xtimer

# this test uses 1000 timers by default. for boards that boards don't have
# enough memory, reduce that to 100 or 20, unless NUMOF_TIMERS has been overridden.
LOW_MEMORY_BOARDS += \
  arduino-mega2560 \
  nucleo-l031k6 \
  #

SUPER_LOW_MEMORY_BOARDS += \
  arduino-uno \
  #

ifneq (, $(filter $(BOARD), $(LOW_MEMORY_BOARDS)))
  NUMOF_TIMERS ?= 100
endif

ifneq (, $(filter $(BOARD), $(SUPER_LOW_MEMORY_BOARDS)))
  NUMOF_TIMERS ?= 20
endif

NUMOF_TIMERS ?= 1000

CFLAGS += -DNUMOF_TIMERS=$(NUMOF_TIMERS)

include $(RIOTBASE)/Makefile.include
