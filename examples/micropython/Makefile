# name of your application
APPLICATION = micropython

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# select MicroPython package
USEPKG += micropython

# include boot.py as header
BLOBS += boot.py

# configure MicroPython's heap size
CFLAGS += '-DMP_RIOT_HEAPSIZE=(8U*1024)'

# MicroPython needs a larger stack
CFLAGS += '-DTHREAD_STACKSIZE_MAIN=THREAD_STACKSIZE_DEFAULT*4'

# use miniterm (instead of the default pyterm) in order to support control
# characters (CTRL-D ...)
RIOT_TERMINAL ?= miniterm

# enable modmachine support for periphials if available
FEATURES_OPTIONAL += periph_adc
FEATURES_OPTIONAL += periph_spi

include $(RIOTBASE)/Makefile.include
