APPLICATION = cortexm_address_check

BOARD_WHITELIST := samr21-xpro iotlab-m3 nucleo-l152re nucleo-l073rz \
                   nucleo-l031k6 nucleo-l053r8 b-l072z-lrwan1 \
                   b-l475e-iot01a nucleo-f091rc nucleo-f103rb \
                   nucleo-f207zg nucleo-f303k8 nucleo-f446re \
                   nucleo-f746zg nucleo-l476rg

BOARD ?= nucleo-l152re

USEMODULE += shell
USEMODULE += shell_commands

include ../Makefile.tests_common

include $(RIOTBASE)/Makefile.include
