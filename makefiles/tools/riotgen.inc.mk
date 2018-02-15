.PHONY: riotgen-installed generate-%

-include makefiles/color.inc.mk

riotgen-installed:
	@command -v riotgen 2>&1 > /dev/null || \
	{ $(COLOR_ECHO) \
	"$(COLOR_RED)'riotgen' command is not available \
	please consider installing it from \
	https://pypi.python.org/pypi/riotgen$(COLOR_RESET)"; \
	exit 1; }

GENERATORS = example test board pkg
$(GENERATORS): %: generate-%

generate-%: riotgen-installed
	@RIOTBASE=. riotgen $*
