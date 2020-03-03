FLASHER = $(RIOTTOOLS)/jlink/jlink.sh
DEBUGGER = $(RIOTTOOLS)/jlink/jlink.sh
DEBUGSERVER = $(RIOTTOOLS)/jlink/jlink.sh
RESET ?= $(RIOTTOOLS)/jlink/jlink.sh

FLASHFILE ?= $(BINFILE)

FFLAGS ?= flash $(FLASHFILE)
DEBUGGER_FLAGS ?= debug $(ELFFILE)
DEBUGSERVER_FLAGS ?= debug-server
RESET_FLAGS ?= reset

JLINK_SERIAL ?= $(DEBUG_ADAPTER_ID)

JLINK_IF ?= SWD
JLINK_RESET_FILE ?= ${RIOTTOOLS}/jlink/reset.seg

# Export JLINK_SERIAL to required targets
JLINK_TARGETS = debug% flash% reset term-rtt
$(call target-export-variables,$(JLINK_TARGETS),JLINK_SERIAL)

# Export JLINK_DEVICE to required targets
$(call target-export-variables,$(JLINK_TARGETS),JLINK_DEVICE)

# Export JLINK_IF to required targets
$(call target-export-variables,$(JLINK_TARGETS),JLINK_IF)

# Export JLINK_RESET_FILE to required targets
$(call target-export-variables,$(JLINK_TARGETS),JLINK_RESET_FILE)
