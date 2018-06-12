BOARD ?= samr21-xpro
include ../Makefile.tests_common


# Normally all boards using `cortexm_common/ldscripts/cortexm.ld` linkerscript
# Only tested on these ones for the moment
BOARD_WHITELIST += iotlab-a8-m3     # cortex-m3
BOARD_WHITELIST += iotlab-m3        # cortex-m3
BOARD_WHITELIST += samr21-xpro      # cortex-m0plus

include $(RIOTBASE)/Makefile.include


# # # # # # # # # # # # # # # # # # #
# Compile time tests for ROM_OFFSET #
# # # # # # # # # # # # # # # # # # #

COMPILE_TESTS = tests-offsets
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
