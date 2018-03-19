FLASHFILE:=$(filter $(HEXFILE) $(BINFILE) $(ELFFILE),$(FFLAGS))
test-murdock:
	cd $(RIOTBASE) && \
		./.murdock test_job \
		$$(realpath --relative-to $(RIOTBASE) $(APPDIR)) \
		$(BOARD) \
		$(FLASHFILE)

# create $(BINDIR)/.test file only if BOARD is in $(TEST_WHITELIST)
link: $(BINDIR)/.test
$(BINDIR)/.test: $(filter clean, $(MAKECMDGOALS))
ifneq (,$(filter $(BOARD), $(TEST_WHITELIST)))
	$(Q)touch $@
else
	$(Q)true
endif
