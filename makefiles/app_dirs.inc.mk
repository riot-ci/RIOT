# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

APP_DIRS := $(dir $(wildcard \
	$(RIOTBASE)/examples/*/Makefile \
	$(RIOTBASE)/tests/*/Makefile    \
	))
