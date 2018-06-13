BOARD ?= samr21-xpro
include ../Makefile.tests_common


# Normally all boards using `cortexm_common/ldscripts/cortexm.ld` linkerscript
# Only tested on these ones for the moment
BOARD_WHITELIST += iotlab-a8-m3     # cortex-m3
BOARD_WHITELIST += iotlab-m3        # cortex-m3
BOARD_WHITELIST += samr21-xpro      # cortex-m0plus

include $(RIOTBASE)/Makefile.include


# # # # # # # # # # # # # # # # # # # # # # # # # # # #
# Compile time tests for ROM_OFFSET and FW_ROM_LENGTH #
# # # # # # # # # # # # # # # # # # # # # # # # # # # #

COMPILE_TESTS = tests-offsets tests-fw_rom_len
all: $(COMPILE_TESTS)
.PHONY: $(COMPILE_TESTS)


# Compile elf files with different ROM_OFFSET
# and verify the offset is taken into account

OFFSETS_TESTS = 0x1000 0x2000
tests-offsets: $(OFFSETS_TESTS:%=test-offset_%)

.PHONY: test-offset_%
test-offset_%: $(BINDIR)/$(APPLICATION)_offset_%.elf
	$(Q)echo -n "Test compilation with offset $*: "
	$(Q)TEST_START_ADDR=$$($(PREFIX)readelf --section-headers $^ 2>/dev/null | awk '/.text/{printf "0x%s\n", $$5}'); \
	EXPECT_START_ADDR=$$(printf "0x%08x" $$(( $(ROM_START_ADDR) + $* ))); \
	if test $${TEST_START_ADDR} != $${EXPECT_START_ADDR}; then \
	  echo "[ERROR] Linker offset not used $${TEST_START_ADDR} != $${EXPECT_START_ADDR}" >&2; \
	  exit 1;\
	fi
	$(Q)@echo [OK]

$(BINDIR)/$(APPLICATION)_offset_%.elf: ROM_OFFSET=$*
$(BINDIR)/$(APPLICATION)_offset_%.elf: $(ELFFILE)
	$(Q)$(_LINK) -o $@
.PRECIOUS: $(BINDIR)/$(APPLICATION)_offset_%.elf


# iotlab-m3 define ROM_LEN as 512K which is not handled by bash math operations
ROM_LEN_BYTES := $(shell printf "0x%x\n" $$(($(ROM_LEN:%K=%*1024))))

# Compile elf files with FW_ROM_LEN and verify the length is taken into account
#   I arbitrarily do 'half' size because I needed to take a value and
#   that it is similar to what is required for doing dual firmware in rom ota

tests-fw_rom_len: test-fw_len_half_rom

.PHONY: test-fw_len_half_rom
test-fw_len_half_rom: $(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf
	$(Q)echo -n "Test compilation with half ROM length: "
	$(Q)TEST_FW_LEN=$$($(PREFIX)readelf --symbols $^ 2>/dev/null | awk '/_fw_rom_length/{printf "0x%s\n", $$2}'); \
	EXPECT_FW_LEN=$$(printf "0x%08x" $$(( $(ROM_LEN_BYTES) / 2 ))); \
	if test $${TEST_FW_LEN} != $${EXPECT_FW_LEN}; then \
	  echo "[ERROR] Linker firmware length not used $${TEST_FW_LEN} != $${EXPECT_FW_LEN}" >&2; \
	  exit 1;\
	fi
	$(Q)@echo [OK]
$(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf: FW_ROM_LEN=$$(($(ROM_LEN_BYTES)/2))
$(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf: $(ELFFILE)
	$(Q)$(_LINK) -o $@
.PRECIOUS: $(BINDIR)/$(APPLICATION)_fw_len_half_rom.elf
