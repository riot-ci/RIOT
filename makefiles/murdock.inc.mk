test-murdock:
	cd $(RIOTBASE) && \
		./.murdock _test \
		$$(realpath --relative-to $(RIOTBASE) $(APPDIR)) \
		$(BOARD) \
		$(HEXFILE)

# create $(BINDIR)/.test file only if BOARD is in $(TEST_WHITELIST)
link: $(BINDIR)/.test
$(BINDIR)/.test: $(filter clean, $(MAKECMDGOALS))
ifneq (,$(filter $(BOARD), $(TEST_WHITELIST)))
	$(Q)touch $@
else
	$(Q)true
endif
