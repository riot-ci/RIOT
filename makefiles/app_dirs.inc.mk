# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

APP_DIR_PATTERNS += \
	$(RIOTBASE)/examples/*/Makefile \
	$(RIOTBASE)/tests/*/Makefile

ifeq (1, $(ENABLE_STAGING))
  include $(RIOTBASE)/staging/staging.mk
  APP_DIR_PATTERNS += $(STAGING_APP_MAKEFILES)
endif

# 1. use wildcard to find Makefiles
# 2. use patsubst to drop trailing "/"
# 3. use patsubst to drop possible leading "./"
# 4. sort
APP_DIRS := $(sort $(patsubst ./%,%,$(patsubst %/,%,$(dir $(wildcard \
	$(APP_DIR_PATTERNS) \
	)))))

info-apps:
	@for dir in $(APP_DIRS); do echo $$dir; done
