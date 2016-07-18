# name of your application
APPLICATION = pkg_micropython

USEPKG += micropython

# If no BOARD is found in the environment, use this default:
BOARD ?= native

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../..

# add directory of generated *.py.h files to include path
CFLAGS += -I$(BINDIR)$(MODULE)py_h/

#
include $(RIOTBASE)/Makefile.include

# generate .py.h header files of .py files
PY = $(wildcard *.py)
PY_H := $(PY:%.py=$(BINDIR)$(MODULE)py_h/%.py.h)

$(BINDIR)$(MODULE)py_h/:
	@mkdir -p $(BINDIR)$(MODULE)py_h/

$(PY_H): $(BINDIR)$(MODULE)py_h/
$(PY_H): $(BINDIR)$(MODULE)py_h/%.py.h: %.py
	xxd -i $< > $@

all: $(PY_H)
