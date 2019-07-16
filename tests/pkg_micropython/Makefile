include ../Makefile.tests_common

# the micropython port currently only supports 32bit architectures
BOARD_BLACKLIST := arduino-duemilanove arduino-leonardo \
                   arduino-mega2560 arduino-nano arduino-uno chronos \
                   jiminy-mega256rfr2 mega-xplained msb-430 msb-430h \
                   telosb waspmote-pro wsn430-v1_3b wsn430-v1_4 z1

# the micropython port depends on ISR_STACKSIZE, which is not available on
# arm7, risc-v
BOARD_BLACKLIST += avsextrem msba2
BOARD_BLACKLIST += hifive1

BOARD_INSUFFICIENT_MEMORY := blackpill bluepill nucleo-f334r8 saml10-xpro \
                             i-nucleo-lrwan1 nucleo-f030r8 nucleo-f031k6 \
                             nucleo-f042k6 nucleo-f302r8 nucleo-f303k8 \
                             nucleo-l031k6 nucleo-l053r8 opencm904


USEPKG += micropython

# MicroPython needs a larger stack
CFLAGS += -DTHREAD_STACKSIZE_MAIN=4096

#
include $(RIOTBASE)/Makefile.include
