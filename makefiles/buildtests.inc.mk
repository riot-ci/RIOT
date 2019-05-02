.PHONY: buildtest

BUILDTEST_MAKE_REDIRECT ?= >/dev/null

ifeq ($(BUILD_IN_DOCKER),1)
buildtest: ..in-docker-container
else
buildtest:
	@ \
	RESULT=true ; \
	for board in $(BOARDS); do \
		$(COLOR_ECHO) -n "Building for $$board ... " ; \
		BOARD=$${board} RIOT_CI_BUILD=1 RIOT_VERSION_OVERRIDE=buildtest \
			$(MAKE) clean $(BUILDTEST_MAKE_REDIRECT); \
		RES=$$? ; \
		if [ $$RES -eq 0 ]; then \
			$(COLOR_ECHO) "$(COLOR_GREEN)success.$(COLOR_RESET)" ; \
		else \
			$(COLOR_ECHO) "$(COLOR_RED)failed!$(COLOR_RESET)" ; \
			RESULT=false ; \
		fi ; \
	done ; \
	$${RESULT}
endif # BUILD_IN_DOCKER
