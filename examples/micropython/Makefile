# name of your application
APPLICATION = micropython

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# boards with too little RAM or flash for MicroPython
BOARD_INSUFFICIENT_MEMORY := blackpill bluepill i-nucleo-lrwan1 nucleo-f030r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f302r8 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l031k6 \
                             nucleo-l053r8 opencm904 saml10-xpro saml11-xpro \
                             stm32f0discovery spark-core stm32l0538-disco

# select MicroPython package
USEPKG += micropython

# include blop.py as header
BLOBS += boot.py

# configure MicroPython's heap size
CFLAGS += '-DMP_RIOT_HEAPSIZE=(8U*1024)'

# MicroPython needs a larger stack
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_DEFAULT*4'

#
include $(RIOTBASE)/Makefile.include
