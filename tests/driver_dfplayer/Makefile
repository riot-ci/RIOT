include ../Makefile.tests_common

USEMODULE += dfplayer
USEMODULE += event_thread_lowest
USEMODULE += shell
USEMODULE += shell_commands

include $(RIOTBASE)/Makefile.include

ifneq (,$(BUSY_PIN))
  CFLAGS += '-DDFPLAYER_PARAM_BUSY_PIN=$(BUSY_PIN)'
endif

ifneq (,$(filter arch_avr8,$(FEATURES_USED)))
  # no strerror() in avrlibc; which makes a lot of sense on a Harvard machine
  CFLAGS += -DDFPLAYER_NO_STRERROR
endif
