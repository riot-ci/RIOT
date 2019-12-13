include ../Makefile.tests_common

USEMODULE += xtimer

# this test uses 1000 timers by default. for boards that boards don't have
# enough memory, reduce that to 100 or 20, unless NUMOF_TIMERS has been overridden.
LOW_MEMORY_BOARDS += \
  airfy-beacon \
  arduino-duemilanove \
  arduino-leonardo \
  arduino-mega2560 \
  b-l072z-lrwan1 \
  blackpill \
  blackpill-128kib \
  bluepill \
  calliope-mini \
  cc1352-launchpad \
  cc2650-launchpad \
  cc2650stk \
  chronos \
  hifive1 \
  hifive1b \
  i-nucleo-lrwan1 \
  lsn50 \
  maple-mini \
  msb-430 \
  nrf51dongle \
  nrf6310 \
  nucleo-f030r8 \
  nucleo-f072rb \
  nucleo-f103rb \
  nucleo-l031k6 \
  nucleo-l053r8 \
  nucleo-l073rz \
  opencm904 \
  saml11-xpro \
  spark-core \
  stm32f0discovery \
  telosb \
  waspmote-pro \
  wsn430-v1_4 \
  z1 \
  #

SUPER_LOW_MEMORY_BOARDS += \
  arduino-duemilanove \
  arduino-nano \
  arduino-uno \
  atmega328p \
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
