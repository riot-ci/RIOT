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

#
include $(RIOTBASE)/Makefile.include

ifneq (, $(filter term, $(MAKECMDGOALS)))
  ifeq (pyterm, $(RIOT_TERMINAL))
    $(warning "Using the MicroPython REPL with pyterm has issues. Better use a different terminal.")
    $(warning "e.g., RIOT_TERMINAL=picocom make ...")
    ifneq (1, $(FORCE_PYTERM))
      $(error "set FORCE_PYTERM=1 to continue (e.g., \"FORCE_PYTERM=1 make term\")")
    endif
  endif
endif
