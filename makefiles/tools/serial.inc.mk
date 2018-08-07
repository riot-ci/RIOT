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
export PORT

export BAUD ?= 115200

RIOT_TERMINAL ?= pyterm
ifeq ($(RIOT_TERMINAL),pyterm)
    export TERMPROG  ?= $(RIOTTOOLS)/pyterm/pyterm
    export TERMFLAGS ?= -p "$(PORT)" -b "$(BAUD)"
else ifeq ($(RIOT_TERMINAL),picocom)
    export TERMPROG  ?= picocom
    export TERMFLAGS ?= --nolock --imap lfcrlf --echo --baud "$(BAUD)" "$(PORT)"
endif

# The SERIAL setting is only available for backwards compatibility with older
# settings.
ifneq (,$(SERIAL))
  SERIAL_TTY = $(firstword $(shell $(RIOTTOOLS)/usb-serial/find-tty.sh $(SERIAL)))
  ifeq (,$(SERIAL_TTY))
    $(error Did not find a device with serial $(SERIAL))
  endif
  PORT_LINUX := $(SERIAL_TTY)
endif
