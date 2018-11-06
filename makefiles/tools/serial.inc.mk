# set default port depending on operating system

ensure_value = $(if $(1),$(1),$(error $(2)))

OS := $(shell uname)
ifeq ($(OS),Linux)
  PORT ?= $(call ensure_value,$(PORT_LINUX),No port set)
else ifeq ($(OS),Darwin)
  PORT ?= $(call ensure_value,$(PORT_DARWIN),No port set)
endif
export BAUD ?= 115200

RIOT_TERMINAL ?= pyterm
ifeq ($(RIOT_TERMINAL),pyterm)
    TERMPROG  ?= $(RIOTTOOLS)/pyterm/pyterm
    TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)"
else ifeq ($(RIOT_TERMINAL),picocom)
    TERMPROG  ?= picocom
    TERMFLAGS ?= --nolock --imap lfcrlf --echo --baud "$(BAUD)" "$(PORT)"
endif
