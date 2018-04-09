
# $1: feature_a:feature_b
# $2: required_features
_conflicting_features = $(strip $(call __conflicting_features,$1,$2))
define __conflicting_features
  $(eval _features := $(subst :, ,$1))
  $(eval _required := $2)
  $(eval _used := $(filter $(_required),$(_features)))
  $(if $(filter 2,$(words $(_used))),$(_features))
endef


EXPECT_ERRORS :=
EXPECT_CONFLICT :=

# Test if there where dependencies against a module in DISABLE_MODULE.

_DISABLED_USEMODULE := $(sort $(filter $(DISABLE_MODULE), $(USEMODULE)))
ifneq (,$(_DISABLED_USEMODULE))
  $(call color_error,"Required modules were disabled using DISABLE_MODULE:", $(_DISABLED_USEMODULE))
  EXPECT_ERRORS := 1
endif

# Test if all feature requirements were met by the selected board.
_MISSING_FEATURES := $(sort $(filter-out $(FEATURES_PROVIDED) $(FEATURES_OPTIONAL),$(FEATURES_REQUIRED)))
ifneq (,$(_MISSING_FEATURES))
  $(call color_error,"There are unsatisfied feature requirements:",$(_MISSING_FEATURES))
  EXPECT_ERRORS := 1
endif

# Test if any required feature conflict with another one.
_CONFLICTING_FEATURES := $(sort $(foreach v,$(FEATURES_CONFLICT),$(call _conflicting_features,$(v),$(FEATURES_REQUIRED))))
ifneq (,$(_CONFLICTING_FEATURES))
  $(call color_warning,"The following features may conflict:",$(_CONFLICTING_FEATURES))
  ifneq (, $(FEATURES_CONFLICT_MSG))
    $(call color_warning,"Rationale:",$(FEATURES_CONFLICT_MSG))
  endif
  EXPECT_CONFLICT := 1
endif

# If there is a whitelist, then test if the board is whitelisted.
ifneq (, $(BOARD_WHITELIST))
  ifeq (, $(filter $(BOARD_WHITELIST), $(BOARD)))
    $(call color_error,"The selected BOARD=$(BOARD) is not whitelisted:",$(BOARD_WHITELIST))
    EXPECT_ERRORS := 1
  endif
endif

# If there is a blacklist, then test if the board is blacklisted.
ifneq (, $(BOARD_BLACKLIST))
  ifneq (, $(filter $(BOARD_BLACKLIST), $(BOARD)))
    $(call color_error,"The selected BOARD=$(BOARD) is blacklisted:",$(BOARD_BLACKLIST))
    EXPECT_ERRORS := 1
  endif
endif

ifneq (, $(EXPECT_CONFLICT))
  $(call color_warning,"\\nEXPECT undesired behaviour!")
endif

ifneq (, $(EXPECT_ERRORS))
  $(call color_error,"\\n\\nEXPECT ERRORS!\n\n")
endif
