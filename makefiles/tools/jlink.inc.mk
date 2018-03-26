export FLASHER = $(RIOTBASE)/dist/tools/jlink/jlink.sh
export DEBUGGER = $(RIOTBASE)/dist/tools/jlink/jlink.sh
export DEBUGSERVER = $(RIOTBASE)/dist/tools/jlink/jlink.sh
export RESET = $(RIOTBASE)/dist/tools/jlink/jlink.sh

HEXFILE = $(BINFILE)

export FFLAGS ?= flash $(HEXFILE)
export DEBUGGER_FLAGS ?= debug $(ELFFILE)
export DEBUGSERVER_FLAGS ?= debug-server
export RESET_FLAGS ?= reset
