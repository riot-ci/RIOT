# name of your application
APPLICATION = micropython

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# the MicroPython port currently only supports 32bit architectures
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano arduino-uno chronos \
                   jiminy-mega256rfr2 mega-xplained msb-430 msb-430h \
                   telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

# the MicroPython port depends on ISR_STACKSIZE, which is not available on
# arm7, risc-v, pic32
BOARD_BLACKLIST += avsextrem msba2
BOARD_BLACKLIST += hifive1
BOARD_BLACKLIST += pic32-clicker pic32-wifire

# boards with too little RAM or flash for MicroPython
BOARD_INSUFFICIENT_MEMORY := blackpill bluepill i-nucleo-lrwan1 nucleo-f030r8 \
                             nucleo-f031k6 nucleo-f042k6 nucleo-f302r8 \
                             nucleo-f303k8 nucleo-f334r8 nucleo-l031k6 \
                             nucleo-l053r8 opencm904 saml10-xpro saml11-xpro \
                             stm32f0discovery spark-core stm32l0538-disco

# select MicroPython package
USEPKG += micropython

# configure MicroPython's heap size
CFLAGS += 'MP_RIOT_HEAPSIZE (8U*1024)'

# MicroPython needs a larger stack
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_DEFAULT*4'

#
include $(RIOTBASE)/Makefile.include
