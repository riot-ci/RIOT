BOARD ?= nucleo-l476rg
include ../Makefile.tests_common

USEPKG += cmsis-nn

BLOBS += input

CFLAGS += -Wno-sign-compare

# Boards that were tested and are known to work
BOARD_WHITELIST := \
  b-l475e-iot01a \
  iotlab-m3 \
  nucleo-l476rg \
  stm32f723e-disco
  #

include $(RIOTBASE)/Makefile.include
