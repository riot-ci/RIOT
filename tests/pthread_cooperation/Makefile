include ../Makefile.tests_common

BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo arduino-mega2560 \
                   arduino-nano arduino-uno hifive1 hifive1b i-nucleo-lrwan1 \
                   jiminy-mega256rfr2 mega-xplained stm32l0538-disco waspmote-pro

# AVR platform: unknown type name: clockid_t
# hifive1: not enough memory for thread stacks

BOARD_INSUFFICIENT_MEMORY := nucleo-f031k6

# nucleo-f303k8 doesn't have enough RAM to run the test so we reduce the stack
# size for every thread
ifneq (,$(filter nucleo-f303k8,$(BOARD)))
  CFLAGS += -DTHREAD_STACKSIZE_DEFAULT=512
endif

USEMODULE += posix_headers
USEMODULE += pthread

TEST_ON_CI_WHITELIST += all

include $(RIOTBASE)/Makefile.include
