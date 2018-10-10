ifneq (,$(filter riotboot,$(FEATURES_PROVIDED)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot0 riotboot/bootloader/%

RIOTBOOT_DIR = $(RIOTBASE)/bootloaders/riotboot
RIOTBOOT ?= $(RIOTBOOT_DIR)/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

HEADER_TOOL_DIR = $(RIOTBASE)/dist/tools/riot_hdr
HEADER_TOOL ?= $(HEADER_TOOL_DIR)/bin/genhdr

# Indicate the reserved space for a header, 256B by default
# Notice that it must be 256B aligned. This is restricted by
# the Cortex-M0+/3/4/7 architecture
RIOTBOOT_HDR_LEN ?= 0x100

# export variables for 'slot_util'
export RIOTBOOT_LEN
export SLOT_LEN

# By default, slot 0 is found just after RIOTBOOT_LEN. It might
# be overridden to add more offset as needed.
export SLOT0_OFFSET ?= $(RIOTBOOT_LEN)

# Mandatory APP_VER, set to 0 by default
APP_VER ?= 0

# For slot generation, one needs only to link
$(BINDIR)/$(APPLICATION)-%.elf: link
	$(Q)$(_LINK) -o $@

# slot 0 targets
SLOT0_FIRMWARE_OFFSET := $$(($(RIOTBOOT_LEN) + $(RIOTBOOT_HDR_LEN)))
$(BINDIR)/$(APPLICATION)-slot%.elf: FW_ROM_LEN=$(SLOT_LEN - $(RIOTBOOT_HDR_LEN))
$(BINDIR)/$(APPLICATION)-slot0.elf: ROM_OFFSET=$(SLOT0_FIRMWARE_OFFSET)

# create binary target with RIOT header
$(BINDIR)/$(APPLICATION)-slot0.riot.bin: %.riot.bin: %.hdr %.bin
	@echo "creating $@..."
	$(Q)cat $^ > $@

# compile header tool if it doesn't exist
$(HEADER_TOOL): FORCE
	@echo "compiling $@..."
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET) \
		PATH=$(PATH) \
			make --no-print-directory -C $(HEADER_TOOL_DIR) all

# Generate RIOT header and keep the original binary file
.PRECIOUS: %.bin
%.hdr: $(HEADER_TOOL) %.bin FORCE
	$(Q)$(HEADER_TOOL) generate $< $(APP_VER) $$(($(ROM_START_ADDR)+$(OFFSET))) $(RIOTBOOT_HDR_LEN) - > $@

$(BINDIR)/$(APPLICATION)-slot0.hdr: OFFSET=$(SLOT0_FIRMWARE_OFFSET)

# Generic target to create a binary file from the image with header
riotboot: $(BINDIR)/$(APPLICATION)-slot0.riot.bin

# riotboot compile target
riotboot/flash-bootloader: riotboot/bootloader/flash
riotboot/bootloader/%:
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET)\
		PATH=$(PATH) BOARD=$(BOARD) \
			make --no-print-directory -C $(RIOTBOOT_DIR) $*

# Generate a binary file from the bootloader which fills all the
# allocated space. This is used afterwards to create a combined
# binary with both bootloader and RIOT image
BOOTLOADER_BIN = $(RIOTBASE)/riotboot/bin/$(BOARD)
$(BOOTLOADER_BIN)/riotboot.extended.bin: $(BOOTLOADER_BIN)/riotboot.bin
	cp $^ $@.tmp
	truncate -s $$(($(RIOTBOOT_LEN))) $@.tmp
	mv $@.tmp $@

# Only call sub make if not already in riotboot
ifneq ($(BOOTLOADER_BIN)/riotboot.bin,$(BINFILE))
  $(BOOTLOADER_BIN)/riotboot.bin: riotboot/bootloader/binfile
endif

# Create combined binary booloader + RIOT image
riotboot/combined-slot0: $(BINDIR)/$(APPLICATION)-slot0-combined.bin
$(BINDIR)/$(APPLICATION)-slot0-combined.bin: $(BOOTLOADER_BIN)/riotboot.extended.bin $(BINDIR)/$(APPLICATION)-slot0.riot.bin
	cat $^ > $@

# Flashing rule for edbg to flash combined binaries
riotboot/flash-combined-slot0: HEXFILE=$(BINDIR)/$(APPLICATION)-slot0-combined.bin
# Flashing rule for openocd to flash combined binaries
riotboot/flash-combined-slot0: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot0-combined.bin
riotboot/flash-combined-slot0: $(BINDIR)/$(APPLICATION)-slot0-combined.bin
	$(FLASHER) $(FFLAGS)

# Flashing rule for slot 0
riotboot/flash-slot0: export IMAGE_OFFSET=$(RIOTBOOT_LEN)
# Flashing rule for edbg to flash only slot 0
riotboot/flash-slot0: HEXFILE=$(BINDIR)/$(APPLICATION)-slot0.riot.bin
# openocd
riotboot/flash-slot0: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot0.riot.bin
riotboot/flash-slot0: $(BINDIR)/$(APPLICATION)-slot0.riot.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

# Targets to generate only slot 0 binary
riotboot/slot0: $(BINDIR)/$(APPLICATION)-slot0.riot.bin

# Default flashing rule for bootloader + slot 0
riotboot/flash: riotboot/flash-slot0

else
riotboot:
	$(Q)echo "error: riotboot feature not selected! (try FEATURES_REQUIRED += riotboot)"
	$(Q)false

endif # (,$(filter riotboot,$(FEATURES_PROVIDED)))
