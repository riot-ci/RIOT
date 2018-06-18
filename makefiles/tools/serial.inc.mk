# set default port depending on operating system
OS := $(shell uname)
ifeq ($(OS),Linux)
  PORT ?= $(PORT_LINUX)
else ifeq ($(OS),Darwin)
  PORT ?= $(PORT_DARWIN)
endif
ifeq ($(PORT),)
  $(info Warning: no PORT set!)
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
