.PHONY: riotgen-installed bootstrap-%

-include makefiles/color.inc.mk

HAVE_RIOTGEN := $(shell which riotgen 2> /dev/null)
riotgen-installed:
ifndef HAVE_RIOTGEN
	@$(COLOR_ECHO) -n '$(COLOR_RED)"riotgen" command is not available, please '
	@$(COLOR_ECHO) -n '$(COLOR_RED)consider installing it from '
	@$(COLOR_ECHO) '$(COLOR_RED)https://pypi.python.org/pypi/riotgen$(COLOR_RESET)'
	@exit 1
endif

GENERATORS = example test board
$(GENERATORS): %: bootstrap-%

bootstrap-%: riotgen-installed
	@RIOTBASE=. riotgen $*
