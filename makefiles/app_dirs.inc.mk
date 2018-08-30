# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

APP_DIRS := $(patsubst %/,%,$(dir $(wildcard \
	$(RIOTBASE)/examples/*/Makefile \
	$(RIOTBASE)/tests/*/Makefile    \
	)))

info-apps:
	@for dir in $(APP_DIRS); do echo $$dir; done
