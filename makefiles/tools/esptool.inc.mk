ifneq (,$(filter esp_log_colored,$(USEMODULE)))
  BOOTLOADER_COLOR = _colors
endif
ifneq (,$(filter esp_log_startup,$(USEMODULE)))
  BOOTLOADER_INFO = _info
endif

BOOTLOADER_BIN = bootloader$(BOOTLOADER_COLOR)$(BOOTLOADER_INFO).bin

ESPTOOL ?= $(RIOTTOOLS)/esptool/esptool.py

# The ELFFILE is the base one used for flashing
ELFFILE ?= $(BINDIR)/$(APPLICATION).elf
FLASHFILE ?= $(ELFFILE)

# Convert .elf and .csv to .bin files at build time. These are needed for
# flashing only, but if they are added to FLASHDEPS instead a "flash-only"
# target would cause a rebuild of $(ELFFILE) since these files depend on it and
# ELFFILE depends on FORCE.
BUILD_FILES += $(FLASHFILE).bin $(BINDIR)/partitions.bin

# flasher configuration
ifneq (,$(filter esp_qemu,$(USEMODULE)))
  FLASHER =
  FFLAGS =
  FLASHDEPS += esp-qemu
else
  PROGRAMMER_SPEED ?= 460800
  FLASHER = $(ESPTOOL)
  FFLAGS += --chip $(FLASH_CHIP) --port $(PROG_DEV) --baud $(PROGRAMMER_SPEED)
  FFLAGS += --before default_reset write_flash -z
  FFLAGS += --flash_mode $(FLASH_MODE) --flash_freq $(FLASH_FREQ)
  FFLAGS += $(BOOTLOADER_POS) $(RIOTCPU)/$(CPU)/bin/$(BOOTLOADER_BIN)
  FFLAGS += 0x8000 $(BINDIR)/partitions.bin
  FFLAGS += 0x10000 $(FLASHFILE).bin
endif

$(FLASHFILE).bin: $(FLASHFILE)
	$(Q)$(ESPTOOL) --chip $(FLASH_CHIP) elf2image --flash_mode $(FLASH_MODE) \
		--flash_size $(FLASH_SIZE)MB --flash_freq $(FLASH_FREQ) $(FLASH_OPTS) \
		-o $@ $<

# Default partition table with no OTA. Can be replaced with a custom partition
# table setting PARTITION_TABLE_CSV.
PARTITION_TABLE_CSV ?= $(BINDIR)/partitions.csv

$(BINDIR)/partitions.csv: $(FLASHFILE).bin
	$(Q)printf "\n" > $(BINDIR)/partitions.csv
	$(Q)printf "nvs, data, nvs, 0x9000, 0x6000\n" >> $@
	$(Q)printf "phy_init, data, phy, 0xf000, 0x1000\n" >> $@
	$(Q)printf "factory, app, factory, 0x10000, " >> $@
	$(Q)ls -l $(FLASHFILE).bin | awk '{ print $$5 }' >> $@

$(BINDIR)/partitions.bin: $(PARTITION_TABLE_CSV)
	$(Q)python3 $(RIOTTOOLS)/esptool/gen_esp32part.py --verify $< $@

.PHONY: esp-qemu

esp-qemu:
	$(Q)dd if=/dev/zero bs=1M count=$(FLASH_SIZE) | \
	  tr "\\000" "\\377" > tmp.bin && cat tmp.bin | \
		head -c $$(($(BOOTLOADER_POS))) | \
		cat - $(RIOTCPU)/$(CPU)/bin/$(BOOTLOADER_BIN) tmp.bin | \
		head -c $$((0x8000)) | \
		cat - $(BINDIR)/partitions.bin tmp.bin | \
		head -c $$((0x10000)) | \
		cat - $(FLASHFILE).bin tmp.bin | \
		head -c $(FLASH_SIZE)MB > $(BINDIR)/$(CPU)flash.bin && rm tmp.bin
ifeq (esp32,$(CPU_FAM))
	$(Q)cp $(RIOTCPU)/$(CPU)/bin/rom_0x3ff90000_0x00010000.bin $(BINDIR)/rom1.bin
	$(Q)cp $(RIOTCPU)/$(CPU)/bin/rom_0x40000000_0x000c2000.bin $(BINDIR)/rom.bin
endif

# reset tool configuration
RESET ?= $(RIOTTOOLS)/esptool/espreset.py
RESET_FLAGS ?= --port $(PROG_DEV)
