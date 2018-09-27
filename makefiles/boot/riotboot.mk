ifneq (,$(filter riotboot,$(FEATURES_PROVIDED)))

.PHONY: riotboot/flash riotboot/flash-bootloader riotboot/flash-slot1 riotboot/bootloader/%

RIOTBOOT_DIR = $(RIOTBASE)/bootloaders/riotboot
RIOTBOOT ?= $(RIOTBOOT_DIR)/bin/$(BOARD)/riotboot.elf
CFLAGS += -I$(BINDIR)/riotbuild

HEADER_TOOL ?= $(RIOTBASE)/dist/tools/riot_hdr/bin/genhdr
RIOTBOOT_HDR_LEN ?= 0x100

# TODO 'genhdr' should be rebuilt here at the same time with the same
# compilation options, to keep them synchronised
CFLAGS += -DRIOT_HDR_SIZE=$(RIOTBOOT_HDR_LEN)

# export variables for 'slot_util'
export RIOTBOOT_SLOT0_SIZE
export RIOTBOOT_FW_SLOT_SIZE

# Mandatory APP_VER, set to 0 by default
APP_VER ?= 0

# For slot generation, one needs only to link
$(BINDIR)/$(APPLICATION)-%.elf: link
	$(Q)$(_LINK) -o $@

# slot 1 targets
SLOT1_OFFSET := $$(($(RIOTBOOT_SLOT0_SIZE) + $(RIOTBOOT_HDR_LEN)))
$(BINDIR)/$(APPLICATION)-slot%.elf: FW_ROM_LEN=$(RIOTBOOT_FW_SLOT_SIZE - $(RIOTBOOT_HDR_LEN))
$(BINDIR)/$(APPLICATION)-slot1.elf: ROM_OFFSET=$(SLOT1_OFFSET)

# create binary target with RIOT header
$(BINDIR)/$(APPLICATION)-slot1.riot.bin: %.riot.bin: %.hdr %.bin
	@echo "creating $@..."
	$(Q)cat $^ > $@

# Generate RIOT header and keep the original binary file
.PRECIOUS: %.bin
%.hdr: %.bin FORCE
	$(Q)$(HEADER_TOOL) generate $< $(APP_VER) $$(($(ROM_START_ADDR)+$(OFFSET))) - > $@

$(BINDIR)/$(APPLICATION)-slot1.hdr: OFFSET=$(SLOT1_OFFSET)

# Generic target to create a binary file from the image with header
riotboot: $(BINDIR)/$(APPLICATION)-slot1.riot.bin

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
	truncate -s $$(($(RIOTBOOT_SLOT0_SIZE))) $@.tmp
	mv $@.tmp $@

# Only call sub make if not already in riotboot
ifneq ($(BOOTLOADER_BIN)/riotboot.bin,$(BINFILE))
  $(BOOTLOADER_BIN)/riotboot.bin: riotboot/bootloader/binfile
endif

# Create combined binary booloader + RIOT image
riotboot/combined-slot1: $(BINDIR)/$(APPLICATION)-slot1-combined.bin
$(BINDIR)/$(APPLICATION)-slot1-combined.bin: $(BOOTLOADER_BIN)/riotboot.extended.bin $(BINDIR)/$(APPLICATION)-slot1.riot.bin
	cat $^ > $@

# Flashing rule for edbg to flash combined binaries
riotboot/flash-combined-slot1: HEXFILE=$(BINDIR)/$(APPLICATION)-slot1-combined.bin
# Flashing rule for openocd to flash combined binaries
riotboot/flash-combined-slot1: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot1-combined.bin
riotboot/flash-combined-slot1: $(BINDIR)/$(APPLICATION)-slot1-combined.bin
	$(FLASHER) $(FFLAGS)

# Flashing rule for slot 1
riotboot/flash-slot1: export IMAGE_OFFSET=$(RIOTBOOT_SLOT0_SIZE)
# Flashing rule for edbg to flash only slot 1
riotboot/flash-slot1: HEXFILE=$(BINDIR)/$(APPLICATION)-slot1.riot.bin
# openocd
riotboot/flash-slot1: export IMAGE_FILE=$(BINDIR)/$(APPLICATION)-slot1.riot.bin
riotboot/flash-slot1: $(BINDIR)/$(APPLICATION)-slot1.riot.bin riotboot/flash-bootloader
	$(FLASHER) $(FFLAGS)

# Targets to generate only slot 1 binary
riotboot/slot1: $(BINDIR)/$(APPLICATION)-slot1.riot.bin

# Default flashing rule for bootloader + slot 1
riotboot/flash: riotboot/flash-slot1

else
riotboot:
	$(Q)echo "error: riotboot feature not selected! (try FEATURES_REQUIRED += riotboot)"
	$(Q)false

endif # (,$(filter riotboot,$(FEATURES_USED)))
