ifneq (,$(filter riotboot,$(FEATURES_USED)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot0 riotboot/bootloader/%

RIOTBOOT_DIR = $(RIOTBASE)/bootloaders/riotboot
RIOTBOOT ?= $(RIOTBOOT_DIR)/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

HEADER_TOOL_DIR = $(RIOTBASE)/dist/tools/riotboot_gen_hdr
HEADER_TOOL ?= $(HEADER_TOOL_DIR)/bin/genhdr
BINDIR_APP = $(BINDIR)/$(APPLICATION)

# Indicate the reserved space for a header, 256B by default
# Notice that it must be 256B aligned. This is restricted by
# the Cortex-M0+/3/4/7 architecture
RIOTBOOT_HDR_LEN ?= 0x100

# Export variables for 'riotboot_slot'
export SLOT0_LEN

# By default, slot 0 is found just after RIOTBOOT_LEN. It might
# be overridden to add more offset as needed.
export SLOT0_OFFSET ?= $(RIOTBOOT_LEN)

# Mandatory APP_VER, set to 0 by default
APP_VER ?= 0

# Final target for slot 0 with riot_hdr
SLOT0_RIOT_BIN = $(BINDIR_APP)-slot0.riot.bin

# For slot generation only link is needed
$(BINDIR_APP)-%.elf: link
	$(Q)$(_LINK) -o $@

# Slot 0 firmware offset, after header
SLOT0_IMAGE_OFFSET := $$(($(RIOTBOOT_LEN) + $(RIOTBOOT_HDR_LEN)))

# Link slots ELF *after* riot_hdr and limit the ROM to the slots length
$(BINDIR_APP)-slot0.elf: FW_ROM_LEN=$$((SLOT0_LEN - $(RIOTBOOT_HDR_LEN)))
$(BINDIR_APP)-slot0.elf: ROM_OFFSET=$(SLOT0_IMAGE_OFFSET)

# Create binary target with RIOT header
$(SLOT0_RIOT_BIN): %.riot.bin: %.hdr %.bin
	@echo "creating $@..."
	$(Q)cat $^ > $@

# Compile header tool if it doesn't exist, force its compilation in case
# some files changed
$(HEADER_TOOL): FORCE
	@echo "compiling $@..."
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET) \
		PATH=$(PATH) \
			$(MAKE) --no-print-directory -C $(HEADER_TOOL_DIR) all

# Generate RIOT header and keep the original binary file
# It must be always regenerated in case of any changes, so FORCE
.PRECIOUS: %.bin
%.hdr: $(HEADER_TOOL) %.bin FORCE
	$(Q)$(HEADER_TOOL) generate $< $(APP_VER) $$(($(ROM_START_ADDR)+$(OFFSET))) $(RIOTBOOT_HDR_LEN) - > $@

$(BINDIR_APP)-slot0.hdr: OFFSET=$(SLOT0_IMAGE_OFFSET)

# Generic target to create a binary file from the image with header
riotboot: $(SLOT0_RIOT_BIN)

# riotboot bootloader compile target
riotboot/flash-bootloader: riotboot/bootloader/flash
riotboot/bootloader/%:
	$(Q)/usr/bin/env -i \
		QUIET=$(QUIET)\
		PATH=$(PATH) BOARD=$(BOARD) \
			$(MAKE) --no-print-directory -C $(RIOTBOOT_DIR) $*

# Generate a binary file from the bootloader which fills all the
# allocated space. This is used afterwards to create a combined
# binary with both bootloader and RIOT firmware with header
BOOTLOADER_BIN = $(RIOTBOOT_DIR)/bin/$(BOARD)
$(BOOTLOADER_BIN)/riotboot.extended.bin: $(BOOTLOADER_BIN)/riotboot.bin
	$(Q)cp $^ $@.tmp
	$(Q)truncate -s $$(($(RIOTBOOT_LEN))) $@.tmp
	$(Q)mv $@.tmp $@

# Only call sub make if not already in riotboot
ifneq ($(BOOTLOADER_BIN)/riotboot.bin,$(BINFILE))
  $(BOOTLOADER_BIN)/riotboot.bin: riotboot/bootloader/binfile
endif

# Create combined binary booloader + RIOT firmware with header
RIOTBOOT_COMBINED_BIN = $(BINDIR_APP)-slot0-combined.bin
riotboot/combined-slot0: $(RIOTBOOT_COMBINED_BIN)
$(RIOTBOOT_COMBINED_BIN): $(BOOTLOADER_BIN)/riotboot.extended.bin $(SLOT0_RIOT_BIN)
	$(Q)cat $^ > $@

# Flashing rule for edbg to flash combined binaries
riotboot/flash-combined-slot0: HEXFILE=$(RIOTBOOT_COMBINED_BIN)
# Flashing rule for openocd to flash combined binaries
riotboot/flash-combined-slot0: export IMAGE_FILE=$(RIOTBOOT_COMBINED_BIN)
riotboot/flash-combined-slot0: $(RIOTBOOT_COMBINED_BIN)
	$(FLASHER) $(FFLAGS)

# Flashing rule for slot 0
riotboot/flash-slot0: export IMAGE_OFFSET=$(SLOT0_OFFSET)
# Flashing rule for edbg to flash only slot 0
riotboot/flash-slot0: HEXFILE=$(SLOT0_RIOT_BIN)
# openocd
riotboot/flash-slot0: export IMAGE_FILE=$(SLOT0_RIOT_BIN)
riotboot/flash-slot0: $(SLOT0_RIOT_BIN)
	$(FLASHER) $(FFLAGS)

# Targets to generate only slot 0 binary
riotboot/slot0: $(SLOT0_RIOT_BIN)

# Default flashing rule for bootloader + slot 0
riotboot/flash: riotboot/flash-slot0 riotboot/flash-bootloader

else
riotboot:
	$(Q)echo "error: riotboot feature not selected! (try FEATURES_REQUIRED += riotboot)"
	$(Q)false

endif # (,$(filter riotboot,$(FEATURES_USED)))
