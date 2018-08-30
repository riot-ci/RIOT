# fallback so empty RIOTBASE won't lead to "/examples/"
RIOTBASE ?= .

APPLICATION_DIR_PATTERNS += \
	$(RIOTBASE)/examples/*/Makefile \
	$(RIOTBASE)/tests/*/Makefile

ifeq (1, $(ENABLE_STAGING))
  include $(RIOTBASE)/staging/staging.mk
  APPLICATION_DIR_PATTERNS += $(STAGING_APPLICATION_MAKEFILES)
endif

# 1. use wildcard to find Makefiles
# 2. use patsubst to drop trailing "/"
# 3. use patsubst to drop possible leading "./"
# 4. sort
APPLICATION_DIRS := $(sort $(patsubst ./%,%,$(patsubst %/,%,$(dir $(wildcard \
	$(APPLICATION_DIR_PATTERNS) \
	)))))

info-applications:
	@for dir in $(APPLICATION_DIRS); do echo $$dir; done
