APPLICATION = hello_mcuboot

BOARD ?= nrf52dk

include ../Makefile.tests_common

BOARD_WHITELIST := nrf52dk

export IMAGE_VERSION = 1.1.1+1
export IMAGE_ALIGN = 8
export IMAGE_HDR = 512

default: mcuboot

include $(RIOTBASE)/Makefile.include
